#include "generator.hpp"

IR_Generator::IR_Generator(AbstractSyntaxTree const &ast, ParsingContext &ctx) : ctx(ctx) {
    setupIntrinsicSignatures();
    iunit = std::make_unique<IntermediateUnit>();
    genTransUnit(*ast.top);
}

void IR_Generator::setupIntrinsicSignatures() {
    intrinsicSignatures[ParsingContext::RESW_BUILTIN_CTZ32] = {
            .op = IR_ExprOper::INTR_CTZ,
            .retType = IR_TypeDirect::getI32(),
            .argsTypes = { IR_TypeDirect::getU32() },
    };
    intrinsicSignatures[ParsingContext::RESW_BUILTIN_CLZ32] = {
            .op = IR_ExprOper::INTR_CLZ,
            .retType = IR_TypeDirect::getI32(),
            .argsTypes = { IR_TypeDirect::getU32() },
    };
    intrinsicSignatures[ParsingContext::RESW_BUILTIN_POPCNT32] = {
            .op = IR_ExprOper::INTR_POPCNT,
            .retType = IR_TypeDirect::getI32(),
            .argsTypes = { IR_TypeDirect::getU32() },
    };
    intrinsicSignatures[ParsingContext::RESW_BUILTIN_BITREV32] = {
            .op = IR_ExprOper::INTR_BITREV,
            .retType = IR_TypeDirect::getU32(),
            .argsTypes = { IR_TypeDirect::getU32() },
    };
}


IR_Block &IR_Generator::curBlock() {
    return *selectedBlock;
}

/** Select block as current (nodes will be inserted in this block) */
void IR_Generator::selectBlock(IR_Block &block) {
    selectedBlock = &block;
}

/** Deselect block in case if current branch is terminated (e.g. by ret) */
void IR_Generator::deselectBlock() {
    selectedBlock = nullptr;
}

std::shared_ptr<IntermediateUnit> IR_Generator::getIR() const {
    return iunit;
}


IR_Generator::ControlStructData::ControlStructData(ControlStructData::LoopBlocks loop)
        : data(loop) {}

IR_Generator::ControlStructData::ControlStructData(ControlStructData::SwitchBlocks sw)
        : data(std::move(sw)) {}

bool IR_Generator::ControlStructData::isLoop() const {
    return std::holds_alternative<LoopBlocks>(data);
}

int IR_Generator::ControlStructData::getExit() const {
    return std::visit([](auto const &e){ return e.exit; }, data);
}

IR_Generator::ControlStructData::LoopBlocks& IR_Generator::ControlStructData::getLoop() {
    return std::get<LoopBlocks>(data);
}

IR_Generator::ControlStructData::SwitchBlocks& IR_Generator::ControlStructData::getSwitch() {
    return std::get<SwitchBlocks>(data);
}

std::optional<IR_Generator::ControlStructData::LoopBlocks> IR_Generator::getTopmostLoop() {
    auto it = std::find_if(activeControls.begin(), activeControls.end(),
                           [](ControlStructData const &a) { return a.isLoop(); });
    if (it == activeControls.end())
        return {};
    else
        return it->getLoop();
}

std::optional<IR_Generator::ControlStructData::LoopBlocks> IR_Generator::getNearestLoop() {
    auto it = std::find_if(activeControls.rbegin(), activeControls.rend(),
                           [](ControlStructData const &a) { return a.isLoop(); });
    if (it == activeControls.rend())
        return {};
    else
        return it->getLoop();
}

IR_Generator::ControlStructData::SwitchBlocks* IR_Generator::getNearestSwitch() {
    auto it = std::find_if(activeControls.rbegin(), activeControls.rend(),
                           [](ControlStructData const &a) { return !a.isLoop(); });
    if (it == activeControls.rend())
        return nullptr;
    else
        return &it->getSwitch();
}

// Generator

void IR_Generator::genTransUnit(AST_TranslationUnit const &tunit) {
    for (auto const &top_instr: tunit.children) {
        if (auto funcDef = dynamic_cast<AST_FunctionDef const *>(top_instr))
            createFunction(*funcDef);
        else if (auto decl = dynamic_cast<AST_Declaration const *>(top_instr))
            insertGlobalDeclaration(*decl);
        else
            throw std::logic_error("Wrong top-level node");
    }
}

void IR_Generator::checkNewGlobalName(string_id_t id, yy::location loc) {
    if (ctx.getReserved(id)) {
        semanticError(std::move(loc), fmt::format(
                "Cannot reuse reserved name ('{}')",
                ctx.getIdentById(id)));
    }
    if (globals.contains(id)) {
        semanticError(std::move(loc), fmt::format(
                "Global variable with such name ('{}') has been already declared",
                ctx.getIdentById(id)));
    }
    if (functions.contains(id)) {
        semanticError(std::move(loc), fmt::format(
                "Function with such name ('{}') has been already declared",
                ctx.getIdentById(id)));
    }
}

/** Insert global variable declaration, new struct type or function prototype */
void IR_Generator::insertGlobalDeclaration(AST_Declaration const &decl) {
    // Typedefs were processed in parser
    if (decl.specifiers->storage_specifier == AST_DeclSpecifiers::ST_TYPEDEF) {
        getPrimaryType(decl.specifiers->type_specifiers); // Save struct type if such is there
        return;
    }

    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);
        string_id_t ident = CoreDriver::getDeclaredIdent(*singleDecl->declarator);

        // Function prototype
        if (varType->type == IR_Type::FUNCTION) {
            if (decl.child->v.size() > 1)
                semanticError(decl.loc, "Only one function can be declared per one declaration");
            if (singleDecl->initializer)
                semanticError(singleDecl->initializer->loc, "Prototypes cannot be initialized");

            auto linkage = getFunLinkage(decl.specifiers->storage_specifier, decl.loc);
            auto &fun = iunit->createPrototype(ctx.getIdentById(ident), linkage, varType);
            functions.emplace(ident, fun.getId());
            continue; // break?
        }

        IRval initVal = singleDecl->initializer ?
                        getInitializerVal(varType, *singleDecl->initializer) :
                        IRval::createZeroinit(varType);

        if (!initVal.isConstant())
            semanticError(singleDecl->loc, "Global variable should be initialized with constant value");

        auto ptrType = std::make_shared<IR_TypePtr>(varType);

        auto linkage = getGVarLinkage(decl.specifiers->storage_specifier, decl.loc);
        if (linkage == IntermediateUnit::VarLinkage::DEFAULT && !singleDecl->initializer)
            linkage =  IntermediateUnit::VarLinkage::TENTATIVE;
        if (linkage == IntermediateUnit::VarLinkage::EXTERN && singleDecl->initializer)
            linkage =  IntermediateUnit::VarLinkage::DEFAULT;

        IRval res = iunit->createGlobal(ctx.getIdentById(ident), ptrType, initVal, linkage);
        globals.emplace(ident, res);
    }
}

static int getFspec(AST_DeclSpecifiers const &declSpec) {
    int fspec = IntermediateUnit::Function::FSPEC_NONE;
    if (declSpec.is_inline)
        fspec = fspec | IntermediateUnit::Function::FSPEC_INLINE;
    if (declSpec.is_pure)
        fspec = fspec | IntermediateUnit::Function::FSPEC_PURE;
    if (declSpec.is_fconst)
        fspec = fspec | IntermediateUnit::Function::FSPEC_FCONST;
    return fspec;
}

/** Create function definition */
void IR_Generator::createFunction(AST_FunctionDef const &def) {
    variables.increaseLevel();

    string_id_t funcIdent = CoreDriver::getDeclaredIdent(*def.decl);
    checkNewGlobalName(funcIdent, def.decl->loc);

    auto fullType = getType(*def.specifiers, *def.decl);
    auto linkage = getFunLinkage(def.specifiers->storage_specifier, def.loc);
    int fspec = getFspec(*def.specifiers);

    curFunc = &iunit->createFunction(ctx.getIdentById(funcIdent), linkage, fspec, fullType);
    functions.emplace(funcIdent, curFunc->getId());
    IR_Block &entryBlock = curFunc->cfg.createBlock();
    curFunc->cfg.entryBlockId = entryBlock.id;
    selectBlock(entryBlock);

    auto declArgs = getDeclaredFuncArgs(*def.decl);
    int curArgNum = 0;
    for (auto const &[argIdent, argType] : declArgs) {
        if (ctx.getReserved(argIdent))
            semanticError(def.decl->loc, "Function parameter cannot have reserved name");
        if (variables.hasOnTop(argIdent))
            semanticError(def.decl->loc, "Variable with such name has been already declared");

        if (IR_TypeDirect::getVoid()->equal(*argType))
            semanticError(def.decl->loc, "Function parameter cannot be 'void'");
        auto argPtrType = std::make_shared<IR_TypePtr>(argType);
        IRval argPtr = emitAlloc(argPtrType, argType);

        emitStore(argPtr, IRval::createFunArg(argType, curArgNum), false);

        variables.put(argIdent, argPtr);
        curArgNum++;
    }

    fillBlock(*def.body);

    if (selectedBlock) { // No terminator
        if (curFunc->getFuncType()->ret->equal(*IR_TypeDirect::getVoid())) {
            selectedBlock->setTerminator(IR_ExprTerminator::RET);
        }
        else {
            semanticError(def.body->loc, "Control reaches end of non-void function");
        }
    }

    for (auto const &[block, label, loc] : danglingGotos) {
        auto it = labels.find(label);
        if (it == labels.end())
            semanticError(loc, "Unknown label");
        curFunc->cfg.linkBlocks(block, it->second);
    }

    curFunc = nullptr;
    variables.decreaseLevel();
    labels.clear();
    danglingGotos.clear();
}

/** Process compound statement */
void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    variables.increaseLevel();
    for (auto const &elem: compStmt.body->v) {
        if (selectedBlock == nullptr) { // Definitly ureachable code
            selectBlock(curFunc->cfg.createBlock());
        }

        if (auto decl = dynamic_cast<AST_Declaration const *>(elem))
            insertDeclaration(*decl);
        else if (auto stmt = dynamic_cast<AST_Stmt const *>(elem))
            insertStatement(*stmt);
        else
            throw std::logic_error("Wrong node in compound statement");
    }
    variables.decreaseLevel();
}

/** Create new local variables */
void IR_Generator::insertDeclaration(AST_Declaration const &decl) {
    // Typedefs were processed in parser
    if (decl.specifiers->storage_specifier == AST_DeclSpecifiers::ST_TYPEDEF) {
        getPrimaryType(decl.specifiers->type_specifiers); // Save struct type if such is there
        return;
    }

    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    bool isStatic = isInList(decl.specifiers->storage_specifier, AST_DeclSpecifiers::ST_EXTERN,
                             AST_DeclSpecifiers::ST_STATIC, AST_DeclSpecifiers::ST_WEAK);
    if (isStatic) {
        insertGlobalDeclaration(decl);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        // If variable has register storage, should at least prohibit addrof for it

        auto varType = getType(*decl.specifiers, *singleDecl->declarator);
        string_id_t ident = CoreDriver::getDeclaredIdent(*singleDecl->declarator);

        if (varType->type == IR_Type::FUNCTION)
            semanticError(singleDecl->declarator->loc, "Functions are not allowed inside compound statements");

        // TODO: check for void allocation (and in globals too)
        auto ptrType = std::make_shared<IR_TypePtr>(varType);

        auto topmostLoop = getTopmostLoop();
        std::optional<IRval> var;
        if (!topmostLoop.has_value()) {
            var = emitAlloc(ptrType, varType);
        }
        else { // Do not create allocations inside loops
            IR_Block &cur = curBlock();
            selectBlock(curFunc->cfg.block(topmostLoop->before));
            var = emitAlloc(ptrType, varType);
            selectBlock(cur);
        }

        if (ctx.getReserved(ident))
            semanticError(singleDecl->loc, "Variable cannot have reserved name");
        if (variables.hasOnTop(ident))
            semanticError(singleDecl->loc, "Variable with such name has been already declared");
        variables.put(ident, *var);

        if (singleDecl->initializer) {
            IRval initVal = getInitializerVal(varType, *singleDecl->initializer);
            emitStore(*var, initVal, false); // TODO: volatile
        }
    }
}

IRval IR_Generator::getInitializerVal(std::shared_ptr<IR_Type> const &type, const AST_Initializer &init) {
    if (init.is_compound) {
        auto const &initLst = dynamic_cast<AST_InitializerList const &>(*init.val);
        return getCompoundVal(type, initLst);
    }
    else {
        IRval res = evalExpr(init.getExpr());
        if (!res.getType()->equal(*type)) {
            res = emitCast(std::move(res), type); // Need to check if implicit conversion valid
        }
        return res;
    }
}

void IR_Generator::insertStatement(const AST_Stmt &stmt) {
    if (auto exprStmt = dynamic_cast<AST_ExprStmt const *>(&stmt)) {
        if (exprStmt->child) // Skip empty statements
            evalExpr(*exprStmt->child);
    }
    else if (auto selStmt = dynamic_cast<AST_SelectionStmt const *>(&stmt))
        insertIfStatement(*selStmt);
    else if (auto iterStmt = dynamic_cast<AST_IterStmt const *>(&stmt))
        insertLoopStatement(*iterStmt);
    else if (auto jmpStmt = dynamic_cast<AST_JumpStmt const *>(&stmt))
        insertJumpStatement(*jmpStmt);
    else if (auto compStmt = dynamic_cast<AST_CompoundStmt const *>(&stmt))
        insertCompoundStatement(*compStmt);
    else if (auto lblStmt = dynamic_cast<AST_LabeledStmt const *>(&stmt))
        insertLabeledStatement(*lblStmt);
    else
        throw std::logic_error("Wrong statement type");
}

void IR_Generator::insertIfStatement(const AST_SelectionStmt &stmt) {
    if (stmt.is_switch) {
        insertSwitchStatement(stmt);
        return;
    }

    IRval cond = evalExpr(*stmt.condition);
    curBlock().setTerminator(IR_ExprTerminator::BRANCH, cond);

    IR_Block &blockTrue = curFunc->cfg.createBlock();
    IR_Block *blockFalse = nullptr;
    IR_Block *blockAfter = nullptr;

    curFunc->cfg.linkBlocks(curBlock(), blockTrue);
    if (stmt.else_body) {
        blockFalse = &curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), *blockFalse);
    }
    else {
        blockAfter = &curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), *blockAfter);
    }

    // Fill 'true' block
    selectBlock(blockTrue);
    if (auto compStmt = dynamic_cast<AST_CompoundStmt const *>(stmt.body))
        fillBlock(*compStmt);
    else
        insertStatement(*stmt.body);

    // Link with 'after' block, if 'true' block not terminated
    if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
        if (!blockAfter)
            blockAfter = &curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), *blockAfter);
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
    }

    if (blockFalse) { // if (stmt.else_body)
        // Fill 'else' block
        selectBlock(*blockFalse);
        if (auto compStmt = dynamic_cast<AST_CompoundStmt const *>(stmt.else_body))
            fillBlock(*compStmt);
        else
            insertStatement(*stmt.else_body);

        // Link with 'after' block, if 'else' block not terminated
        if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
            if (!blockAfter)
                blockAfter = &curFunc->cfg.createBlock();
            curFunc->cfg.linkBlocks(curBlock(), *blockAfter);
            curBlock().setTerminator(IR_ExprTerminator::JUMP);
        }
    }

    // Select 'after' block, if such exists
    if (blockAfter)
        selectBlock(*blockAfter);
    else
        deselectBlock();
}

void IR_Generator::insertSwitchStatement(const AST_SelectionStmt &stmt) {
    IRval condVal = evalExpr(*stmt.condition);

    IR_Block &entryBlock = curBlock();
    IR_Block &switchedBlock = curFunc->cfg.createBlock();
    IR_Block &blockAfter = curFunc->cfg.createBlock();

    auto compStmt = dynamic_cast<AST_CompoundStmt const *>(stmt.body);
    if (!compStmt)
        semanticError(stmt.body->loc, "Switch statement can have only compound statement child");

    activeControls.emplace_back(ControlStructData::SwitchBlocks{
        .exit = blockAfter.id,
        .labels = {},
        .defaultBlock = {},
    });
    selectBlock(switchedBlock);
    fillBlock(*compStmt);
    if (selectedBlock != nullptr)
        curFunc->cfg.linkBlocks(curBlock(), blockAfter);

    // Maybe use corresponding LLVM instruction instead or use binsearch
    auto nearestSwitch = getNearestSwitch();
    std::set<IRval> usedCases;
    selectBlock(entryBlock);
    for (auto const &label : nearestSwitch->labels) {
        if (usedCases.contains(label.val))
            semanticError(stmt.loc, "Switch statement has two identical labels");
        usedCases.insert(label.val);

        IRval eqCond = emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { condVal, label.val });
        curBlock().setTerminator(IR_ExprTerminator::BRANCH, eqCond);
        curFunc->cfg.linkBlocks(curBlock(), curFunc->cfg.block(label.block));
        IR_Block &nextCaseBlock = curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), nextCaseBlock);
        selectBlock(nextCaseBlock);
    }

    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    if (nearestSwitch->defaultBlock.has_value()) {
        IR_Block &defaultBlock = curFunc->cfg.block(nearestSwitch->defaultBlock.value());
        curFunc->cfg.linkBlocks(curBlock(), defaultBlock);
    }
    else {
        curFunc->cfg.linkBlocks(curBlock(), blockAfter);
    }

    activeControls.pop_back();

    if (!blockAfter.prev.empty())
        selectBlock(blockAfter);
    else
        deselectBlock();
}

void IR_Generator::insertLoopStatement(const AST_IterStmt &stmt) {
    IR_Block &blockCond = curFunc->cfg.createBlock();
    IR_Block &blockLoop = curFunc->cfg.createBlock();
    IR_Block &blockAfter = curFunc->cfg.createBlock();

    IR_Block &blockBefore = curBlock();

    // Create for-loop initializer
    if (stmt.type == AST_IterStmt::FOR_LOOP) {
        variables.increaseLevel();

        // If 'for' loop hasn't initializer, it represented as empty statement
        auto const &preAction = *stmt.getForLoopControls().decl;
        if (auto expr = dynamic_cast<AST_ExprStmt const *>(&preAction))
            insertStatement(*expr);
        else if (auto decl = dynamic_cast<AST_Declaration const *>(&preAction))
            insertDeclaration(*decl);
        else
            throw std::logic_error("Wrong for-loop pre action type");
    }

    // Make jump to condition
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    if (stmt.type == AST_IterStmt::DO_LOOP)
        curFunc->cfg.linkBlocks(curBlock(), blockLoop);
    else
        curFunc->cfg.linkBlocks(curBlock(), blockCond);

    // Create condition
    selectBlock(blockCond);
    auto const &condNode = stmt.getCond();
    // In 'for' loops condition can be absent
    IRval cond = condNode ? evalExpr(*condNode) : IRval::createVal(IR_TypeDirect::getI32(), 1);
    curBlock().setTerminator(IR_ExprTerminator::BRANCH, cond);
    curFunc->cfg.linkBlocks(curBlock(), blockLoop);
    curFunc->cfg.linkBlocks(curBlock(), blockAfter);

    // Create last action
    if (stmt.type == AST_IterStmt::FOR_LOOP) {
        if (stmt.getForLoopControls().action) {
            auto &blockLastAct = curFunc->cfg.createBlock();
            blockLastAct.setTerminator(IR_ExprTerminator::JUMP);
            curFunc->cfg.linkBlocks(blockLastAct, blockCond);
            selectBlock(blockLastAct);
            evalExpr(*stmt.getForLoopControls().action);

            activeControls.emplace_back(ControlStructData::LoopBlocks{
                    .skip = blockLastAct.id,
                    .exit = blockAfter.id,
                    .before = blockBefore.id });
        }
        else { // No last action
            activeControls.emplace_back(ControlStructData::LoopBlocks{
                    .skip = blockCond.id,
                    .exit = blockAfter.id,
                    .before = blockBefore.id });
        }
    }
    else { // WHILE_LOOP, DO_LOOP
        activeControls.emplace_back(ControlStructData::LoopBlocks{
                .skip = blockCond.id,
                .exit = blockAfter.id,
                .before = blockBefore.id });
    }

    // Fill body
    selectBlock(blockLoop);
    if (auto compStmt = dynamic_cast<AST_CompoundStmt const *>(stmt.body))
        fillBlock(*compStmt);
    else
        insertStatement(*stmt.body);

    // Terminate body
    if (selectedBlock != nullptr) {
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        curFunc->cfg.linkBlocks(curBlock().id, activeControls.back().getLoop().skip);
    }
    selectBlock(blockAfter);

    activeControls.pop_back();
    if (stmt.type == AST_IterStmt::FOR_LOOP)
        variables.decreaseLevel();
}

void IR_Generator::insertJumpStatement(const AST_JumpStmt &stmt) {
    if (stmt.type == AST_JumpStmt::J_RET) {
        auto const &arg = stmt.getExpr();
        auto funcType = curFunc->getFuncType();
        if (arg) {
            IRval retVal = evalExpr(*arg);
            if (!funcType->ret->equal(*retVal.getType())) {
                retVal = emitCast(std::move(retVal), funcType->ret); // Need to check if implicit conversion valid
            }
            curBlock().setTerminator(IR_ExprTerminator::RET, retVal);
        }
        else {
            if (!funcType->ret->equal(*IR_TypeDirect::getVoid()))
                semanticError(stmt.loc, "Cannot return value in void function");
            curBlock().setTerminator(IR_ExprTerminator::RET);
        }
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_BREAK) {
        if (activeControls.empty())
            semanticError(stmt.loc, "Break keyword outside of loop or switch");
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        curFunc->cfg.linkBlocks(curBlock().id, activeControls.back().getExit());
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_CONTINUE) {
        auto nearestLoop = getNearestLoop();
        if (!nearestLoop.has_value())
            semanticError(stmt.loc, "Continue keyword outside of loop");
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        curFunc->cfg.linkBlocks(curBlock().id, nearestLoop->skip);
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_GOTO) {
        curFunc->fspec |= IntermediateUnit::Function::FSPEC_GOTOED;

        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        string_id_t label = stmt.getIdent();
        if (auto it = labels.find(label); it != labels.end())
            curFunc->cfg.linkBlocks(curBlock().id, it->second);
        else
            danglingGotos.emplace_back(curBlock().id, label, stmt.loc);
        deselectBlock();
    }
    else {
        throw std::logic_error("Wrong jump statement");
    }
}

/** Create new scope and fill block from compound statement */
void IR_Generator::insertCompoundStatement(const AST_CompoundStmt &stmt) {
    if (stmt.body->v.empty())
        return;

    auto &compoundBlock = curFunc->cfg.createBlock();
    curFunc->cfg.linkBlocks(curBlock(), compoundBlock);
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    selectBlock(compoundBlock);
    fillBlock(stmt);

    if (selectedBlock != nullptr) {
        auto &blockAfter = curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), blockAfter);
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        selectBlock(blockAfter);
    }
}

void IR_Generator::insertLabeledStatement(const AST_LabeledStmt &stmt) {
    // Create new block
    if (!curBlock().getAllNodes().empty()) {
        IR_Block &nextBlock = curFunc->cfg.createBlock();
        curFunc->cfg.linkBlocks(curBlock(), nextBlock);
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        selectBlock(nextBlock);
    }

    if (stmt.type == AST_LabeledStmt::SIMPL) {
        string_id_t ident = stmt.getIdent();
        auto lIt = labels.lower_bound(ident);
        if (lIt->first == ident)
            semanticError(stmt.loc, "Such label already exists");
        labels.emplace_hint(lIt, ident, curBlock().id);
    }
    else if (stmt.type == AST_LabeledStmt::SW_CASE) {
        auto *nearestSwitch = getNearestSwitch();
        if (nearestSwitch == nullptr)
            semanticError(stmt.loc, "Case label outside of switch statement");
        IRval caseVal = evalExpr(stmt.getExpr());
        if (caseVal.getValueClass() != IRval::VAL)
            semanticError(stmt.loc, "Only constants can be used in case labels");

        nearestSwitch->labels.push_back({ .val = std::move(caseVal), .block = curBlock().id });
    }
    else if (stmt.type == AST_LabeledStmt::SW_DEFAULT) {
        auto *nearestSwitch = getNearestSwitch();
        if (nearestSwitch == nullptr)
            semanticError(stmt.loc, "Default label outside of switch statement");

        if (nearestSwitch->defaultBlock.has_value())
            semanticError(stmt.loc, "Default velue for switch was already set");
        nearestSwitch->defaultBlock = curBlock().id;
    }
    else {
        throw std::logic_error("Wrong label type");
    }

    insertStatement(*stmt.child);
}

// Semantic exception

void IR_Generator::semanticError(yy::location loc, std::string msg) {
    throw semantic_exception(ctx, std::move(loc), std::move(msg));
}

static std::string formExcLoc(ParsingContext const &ctx, yy::location const &loc) {
    auto trueLoc = ctx.warps.getLoc(loc.begin.line);
    std::string filename = ctx.warps.getFilename(trueLoc.filenum);
    return fmt::format("{}:{}:{}", filename, trueLoc.line, loc.begin.column);
}

IR_Generator::semantic_exception::semantic_exception(ParsingContext &ctx, yy::location const &loc, std::string msg)
        : cw39_exception("Semantic error", formExcLoc(ctx, loc), std::move(msg)) {}
