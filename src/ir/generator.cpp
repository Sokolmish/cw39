#include "generator.hpp"
#include "constants_folder.hpp"

IR_Generator::IR_Generator(CoreDriver &parser, ParsingContext &ctx) : ctx(ctx) {
    cfg = std::make_unique<ControlFlowGraph>();
    genTransUnit(parser);
}


void IR_Generator::semanticError(yy::location loc, const std::string &msg) {
    auto true_loc = ctx.warps.getLoc(loc.begin.line);
    std::string filename = ctx.warps.getFilename(true_loc.filenum);
    fmt::print(stderr, "Semantic error ({}:{}:{}):\n\t{}\n",
               filename, true_loc.line, loc.begin.column, msg);
    exit(EXIT_FAILURE);
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

std::shared_ptr<ControlFlowGraph> IR_Generator::getCfg() const {
    return cfg;
}


IR_Generator::ControlStructData::ControlStructData(IR_Generator::ControlStructData::LoopBlocks loop)
        : data(loop) {}

IR_Generator::ControlStructData::ControlStructData(IR_Generator::ControlStructData::SwitchBlocks sw)
        : data(std::move(sw)) {}

bool IR_Generator::ControlStructData::isLoop() const {
    return std::holds_alternative<LoopBlocks>(data);
}

int IR_Generator::ControlStructData::getExit() const {
    return std::visit([](auto const &e){ return e.exit; }, data);
}

IR_Generator::ControlStructData::LoopBlocks &IR_Generator::ControlStructData::getLoop() {
    return std::get<LoopBlocks>(data);
}

IR_Generator::ControlStructData::SwitchBlocks &IR_Generator::ControlStructData::getSwitch() {
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

IR_Generator::ControlStructData::SwitchBlocks *IR_Generator::getNearestSwitch() {
    auto it = std::find_if(activeControls.rbegin(), activeControls.rend(),
                           [](ControlStructData const &a) { return !a.isLoop(); });
    if (it == activeControls.rend())
        return nullptr;
    else
        return &it->getSwitch();
}


std::optional<IRval> IR_Generator::emitNode(std::optional<IRval> ret, std::unique_ptr<IR_Expr> expr) {
    // TODO: check if in global context
    curBlock().addNode(ret, std::move(expr));
    return ret;
}

std::optional<IRval> IR_Generator::emitNode(std::shared_ptr<IR_Type> ret, std::unique_ptr<IR_Expr> expr) {
    // Store and non-pure calls will never be folded
    auto val = ConstantsFolder::foldExpr(*expr);
    if (val.has_value()) {
        return val;
    }
    else {
        std::optional<IRval> res = {};
        if (ret)
            res = cfg->createReg(std::move(ret));
        return emitNode(res, std::move(expr));
    }
}

IRval IR_Generator::emitOp(std::shared_ptr<IR_Type> ret, IR_ExprOper::IR_Ops op, std::vector<IRval> args) {
    return *emitNode(std::move(ret), std::make_unique<IR_ExprOper>(op, std::move(args)));
}

std::optional<IRval> IR_Generator::emitMov(IRval dst, IRval src) {
    auto expr = std::make_unique<IR_ExprOper>(IR_ExprOper::MOV, std::vector<IRval>{ std::move(src) });
    return emitNode(std::move(dst), std::move(expr));
}

void IR_Generator::emitStore(IRval addr, IRval val) {
    auto expr = std::make_unique<IR_ExprMem>(IR_ExprMem::STORE, std::move(addr), std::move(val));
    emitNode(std::nullopt, std::move(expr));
}

IRval IR_Generator::emitLoad(std::shared_ptr<IR_Type> ret, IRval addr) {
    auto expr = std::make_unique<IR_ExprMem>(IR_ExprMem::LOAD, std::move(addr));
    return *emitNode(std::move(ret), std::move(expr));
}

IRval IR_Generator::emitCast(IRval srcVal, std::shared_ptr<IR_Type> dst) {
    auto dstCopy = dst;
    auto expr = std::make_unique<IR_ExprCast>(std::move(srcVal), std::move(dst));
    return *emitNode(std::move(dstCopy), std::move(expr));
}

std::optional<IRval>
IR_Generator::emitCall(std::shared_ptr<IR_Type> ret, int callee, std::vector<IRval> args) {
    return emitNode(std::move(ret), std::make_unique<IR_ExprCall>(callee, std::move(args)));
}

std::optional<IRval>
IR_Generator::emitIndirCall(std::shared_ptr<IR_Type> ret, IRval callee, std::vector<IRval> args) {
    return emitNode(std::move(ret), std::make_unique<IR_ExprCall>(std::move(callee), std::move(args)));
}

IRval IR_Generator::emitAlloc(std::shared_ptr<IR_Type> ret, std::shared_ptr<IR_Type> type, bool onHeap) {
    return *emitNode(std::move(ret), std::make_unique<IR_ExprAlloc>(std::move(type), onHeap));
}

IRval IR_Generator::emitExtract(std::shared_ptr<IR_Type> ret, IRval base, std::vector<IRval> indices) {
    return *emitNode(std::move(ret), std::make_unique<IR_ExprAccess>(
            IR_ExprAccess::EXTRACT, std::move(base), std::move(indices)));
}

IRval IR_Generator::emitInsert(std::shared_ptr<IR_Type> ret, IRval base, IRval val, std::vector<IRval> ind) {
    return *emitNode(std::move(ret), std::make_unique<IR_ExprAccess>(
            IR_ExprAccess::INSERT, std::move(base), std::move(val), std::move(ind)));
}

IRval IR_Generator::emitGEP(std::shared_ptr<IR_Type> ret, IRval base, std::vector<IRval> indices) {
    return *emitNode(std::move(ret), std::make_unique<IR_ExprAccess>(
            IR_ExprAccess::GEP, std::move(base), std::move(indices)));
}


// Generator

void IR_Generator::genTransUnit(CoreDriver &parser) {
    for (const auto &top_instr: parser.getTransUnit()->children) {
        if (top_instr->node_type == AST_FUNC_DEF)
            createFunction(dynamic_cast<AST_FunctionDef &>(*top_instr));
        else if (top_instr->node_type == AST_DECLARATION)
            insertGlobalDeclaration(dynamic_cast<AST_Declaration const &>(*top_instr));
        else
            internalError("Wrong top-level instruction");
    }
}

/** Insert global variable declaration, new struct type or function prototype */
void IR_Generator::insertGlobalDeclaration(const AST_Declaration &decl) {
    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);

        // Function prototype
        if (varType->type == IR_Type::FUNCTION) {
            if (decl.child->v.size() > 1)
                semanticError(decl.loc, "Only one function can be declared per one declaration");
            if (singleDecl->initializer)
                semanticError(singleDecl->initializer->loc, "Prototypes cannot be initialized");

            // TODO
            auto funcIdent = getDeclaredIdent(*singleDecl->declarator);
            auto fun = cfg->createPrototype(ctx.getIdentById(funcIdent),
                                            IR_StorageSpecifier::EXTERN, varType);
            functions.emplace(funcIdent, fun.getId());
            continue; // break?
        }

        auto ident = getDeclaredIdent(*singleDecl->declarator);
        if (globals.contains(ident)) // TODO: check in functions (not only there)
            semanticError(singleDecl->loc, "Global variable already declared");

        IRval initVal = singleDecl->initializer ?
                        getInitializerVal(varType, *singleDecl->initializer) :
                        IRval::createZeroinit(varType);

        if (!initVal.isConstant())
            semanticError(singleDecl->loc, "Global variable should be initialized with constant value");

        auto ptrType = std::make_shared<IR_TypePtr>(varType);
        IRval res = cfg->createGlobal(ctx.getIdentById(ident), ptrType, initVal);
        globals.emplace(ident, res);
    }
}

/** Convert storage specifier from AST enum to IR one */
static IR_StorageSpecifier storageSpecFromAst(AST_DeclSpecifiers::StorageSpec const &spec) {
    switch (spec) {
        case AST_DeclSpecifiers::ST_AUTO:
            return IR_StorageSpecifier::AUTO;
        case AST_DeclSpecifiers::ST_EXTERN:
            return IR_StorageSpecifier::EXTERN;
        case AST_DeclSpecifiers::ST_STATIC:
            return IR_StorageSpecifier::STATIC;
        case AST_DeclSpecifiers::ST_REGISTER:
            return IR_StorageSpecifier::REGISTER;
        default:
            internalError("Wrong storage specifier");
    }
}

/** Create function definition */
void IR_Generator::createFunction(AST_FunctionDef const &def) {
    variables.increaseLevel();

    IR_StorageSpecifier storage;
    bool isInline;
    std::shared_ptr<IR_Type> fullType;

    if (def.specifiers->storage_specifier == AST_DeclSpecifiers::ST_NONE)
        storage = IR_StorageSpecifier::EXTERN;
    else
        storage = storageSpecFromAst(def.specifiers->storage_specifier);
    isInline = def.specifiers->is_inline;
    fullType = getType(*def.specifiers, *def.decl);

    auto funcIdent = getDeclaredIdent(*def.decl);
    auto fun = cfg->createFunction(ctx.getIdentById(funcIdent), storage, isInline, fullType);
    functions.emplace(funcIdent, fun.getId());
    selectBlock(cfg->block(fun.getEntryBlockId()));
    curFunctionType = std::dynamic_pointer_cast<IR_TypeFunc>(fun.fullType);

    auto declArgs = getDeclaredFuncArgs(*def.decl);
    int curArgNum = 0;
    for (auto const &[argIdent, argType] : declArgs) {
        auto argPtrType = std::make_shared<IR_TypePtr>(argType);
        IRval argPtr = emitAlloc(argPtrType, argType);

        auto argVal = IRval::createFunArg(argType, curArgNum);
        emitStore(argPtr, argVal);

        variables.put(argIdent, argPtr);
        curArgNum++;
    }

    fillBlock(*def.body);
    curFunctionType = nullptr;
    variables.decreaseLevel();

    for (auto const &[block, label, loc] : danglingGotos) {
        auto it = labels.find(label);
        if (it == labels.end())
            semanticError(loc, "Unknown label");
        cfg->linkBlocks(cfg->block(block), cfg->block(it->second));
    }
    labels.clear();
    danglingGotos.clear();
}

/** Process compound statement */
void IR_Generator::fillBlock(const AST_CompoundStmt &compStmt) {
    variables.increaseLevel();
    for (auto const &elem: compStmt.body->v) {
        if (elem->node_type == AST_DECLARATION)
            insertDeclaration(dynamic_cast<AST_Declaration const &>(*elem));
        else if (elem->node_type == AST_STATEMENT)
            insertStatement(dynamic_cast<AST_Stmt const &>(*elem));
        else
            internalError("Wrong node in compound statement");

        // Ureachable code
        if (selectedBlock == nullptr) {
            IR_Block &unreachBlock = cfg->createBlock();
            selectBlock(unreachBlock);
        }
    }
    variables.decreaseLevel();
}

/** Create new local variables */
void IR_Generator::insertDeclaration(AST_Declaration const &decl) {
    // Save struct type if such is there
    if (!decl.child) {
        getPrimaryType(decl.specifiers->type_specifiers);
        return;
    }

    for (const auto &singleDecl : decl.child->v) {
        auto varType = getType(*decl.specifiers, *singleDecl->declarator);
        string_id_t ident = getDeclaredIdent(*singleDecl->declarator);

        if (varType->type == IR_Type::FUNCTION)
            semanticError(singleDecl->declarator->loc, "Functions are not allowed inside compound statements");

        // TODO: check for void allocation (and in globals too)
        std::shared_ptr<IR_Type> ptrType = std::make_shared<IR_TypePtr>(varType);

        auto topmostLoop = getTopmostLoop();
        std::optional<IRval> var;
        if (!topmostLoop.has_value()) {
            var = emitAlloc(ptrType, varType);
        }
        else { // Do not create allocations inside loops
            IR_Block &cur = curBlock();
            selectBlock(cfg->block(topmostLoop->before));
            var = emitAlloc(ptrType, varType);
            selectBlock(cur);
        }

        if (variables.hasOnTop(ident))
            semanticError(singleDecl->loc, "Variable already declared");
        variables.put(ident, *var);

        if (singleDecl->initializer) {
            IRval initVal = getInitializerVal(varType, *singleDecl->initializer);
            emitStore(*var, initVal);
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
            semanticError(init.loc, "Cannot initialize variable with different type");
        }
        return res;
    }
}

void IR_Generator::insertStatement(const AST_Stmt &stmt) {
    if (stmt.type == AST_Stmt::EXPR) {
        auto const &exprStmt = static_cast<AST_ExprStmt const &>(stmt);
        if (exprStmt.child) // Skip empty statements
            evalExpr(*exprStmt.child);
    }
    else if (stmt.type == AST_Stmt::SELECT)
        insertIfStatement(static_cast<AST_SelectionStmt const &>(stmt));
    else if (stmt.type == AST_Stmt::ITER)
        insertLoopStatement(static_cast<AST_IterStmt const &>(stmt));
    else if (stmt.type == AST_Stmt::JUMP)
        insertJumpStatement(static_cast<AST_JumpStmt const &>(stmt));
    else if (stmt.type == AST_Stmt::COMPOUND)
        insertCompoundStatement(static_cast<AST_CompoundStmt const &>(stmt));
    else if (stmt.type == AST_Stmt::LABEL)
        insertLabeledStatement(static_cast<AST_LabeledStmt const &>(stmt));
    else
        internalError("Wrong statement type");
}

void IR_Generator::insertIfStatement(const AST_SelectionStmt &stmt) {
    if (stmt.is_switch) {
        insertSwitchStatement(stmt);
        return;
    }

    IRval cond = evalExpr(*stmt.condition);
    curBlock().setTerminator(IR_ExprTerminator::BRANCH, cond);

    IR_Block &blockTrue = cfg->createBlock();
    IR_Block *blockFalse = nullptr;
    IR_Block *blockAfter = nullptr;

    cfg->linkBlocks(curBlock(), blockTrue);
    if (stmt.else_body) {
        blockFalse = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockFalse);
    }
    else {
        blockAfter = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockAfter);
    }

    // Fill 'true' block
    selectBlock(blockTrue);
    if (stmt.body->type == AST_Stmt::COMPOUND)
        fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
    else
        insertStatement(*stmt.body);

    // Link with 'after' block, if 'true' block not terminated
    if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
        if (!blockAfter)
            blockAfter = &cfg->createBlock();
        cfg->linkBlocks(curBlock(), *blockAfter);
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
    }

    if (blockFalse) { // if (stmt.else_body)
        // Fill 'else' block
        selectBlock(*blockFalse);
        if (stmt.else_body->type == AST_Stmt::COMPOUND)
            fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.else_body));
        else
            insertStatement(*stmt.else_body);

        // Link with 'after' block, if 'else' block not terminated
        if (selectedBlock != nullptr) { // !curBlock().termNode.has_value()
            if (!blockAfter)
                blockAfter = &cfg->createBlock();
            cfg->linkBlocks(curBlock(), *blockAfter);
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
    IR_Block &switchedBlock = cfg->createBlock();
    IR_Block &blockAfter = cfg->createBlock();

    if (stmt.body->type != AST_Stmt::COMPOUND)
        semanticError(stmt.body->loc, "Switch statement can only has compound statement child");

    activeControls.emplace_back(ControlStructData::SwitchBlocks{
        .exit = blockAfter.id,
        .labels = {},
        .defaultBlock = {},
    });
    selectBlock(switchedBlock);
    fillBlock(dynamic_cast<AST_CompoundStmt const &>(*stmt.body));
    if (selectedBlock != nullptr)
        cfg->linkBlocks(curBlock(), blockAfter);

    // TODO: use corresponding LLVM instruction instead or use binsearch
    auto nearestSwitch = getNearestSwitch();
    std::set<IRval, IRval::Comparator> usedCases;
    selectBlock(entryBlock);
    for (auto const &label : nearestSwitch->labels) {
        if (usedCases.contains(label.val))
            semanticError(stmt.loc, "Switch statement has two identical labels");
        usedCases.insert(label.val);

        IRval eqCond = emitOp(IR_TypeDirect::getI1(), IR_ExprOper::EQ, { condVal, label.val });
        curBlock().setTerminator(IR_ExprTerminator::BRANCH, eqCond);
        cfg->linkBlocks(curBlock(), cfg->block(label.block));
        IR_Block &nextCaseBlock = cfg->createBlock();
        cfg->linkBlocks(curBlock(), nextCaseBlock);
        selectBlock(nextCaseBlock);
    }

    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    if (nearestSwitch->defaultBlock.has_value()) {
        IR_Block &defaultBlock = cfg->block(nearestSwitch->defaultBlock.value());
        cfg->linkBlocks(curBlock(), defaultBlock);
    }
    else {
        cfg->linkBlocks(curBlock(), blockAfter);
    }

    activeControls.pop_back();

    if (!blockAfter.prev.empty())
        selectBlock(blockAfter);
    else
        deselectBlock();
}

void IR_Generator::insertLoopStatement(const AST_IterStmt &stmt) {
    IR_Block &blockCond = cfg->createBlock();
    IR_Block &blockLoop = cfg->createBlock();
    IR_Block &blockAfter = cfg->createBlock();

    IR_Block &blockBefore = curBlock();

    // Create for-loop initializer
    if (stmt.type == AST_IterStmt::FOR_LOOP) {
        variables.increaseLevel();

        // If 'for' loop hasn't initializer, it represented as empty statement
        auto const &preAction = *stmt.getForLoopControls().decl;
        if (preAction.node_type == AST_STATEMENT)
            insertStatement(dynamic_cast<AST_ExprStmt const &>(preAction));
        else if (preAction.node_type == AST_DECLARATION)
            insertDeclaration(dynamic_cast<AST_Declaration const &>(preAction));
        else
            internalError("Wrong for-loop pre action type");
    }

    // Make jump to condition
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    if (stmt.type == AST_IterStmt::DO_LOOP)
        cfg->linkBlocks(curBlock(), blockLoop);
    else
        cfg->linkBlocks(curBlock(), blockCond);

    // Create condition
    selectBlock(blockCond);
    auto const &condNode = stmt.getCond();
    // In 'for' loops condition can be absent
    IRval cond = condNode ? evalExpr(*condNode) : IRval::createVal(IR_TypeDirect::getI32(), 1);
    curBlock().setTerminator(IR_ExprTerminator::BRANCH, cond);
    cfg->linkBlocks(curBlock(), blockLoop);
    cfg->linkBlocks(curBlock(), blockAfter);

    // Create last action
    if (stmt.type == AST_IterStmt::FOR_LOOP) {
        if (stmt.getForLoopControls().action) {
            auto &blockLastAct = cfg->createBlock();
            blockLastAct.setTerminator(IR_ExprTerminator::JUMP);
            cfg->linkBlocks(blockLastAct, blockCond);
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
    if (stmt.body->type == AST_Stmt::COMPOUND)
        fillBlock(static_cast<AST_CompoundStmt const &>(*stmt.body));
    else
        insertStatement(*stmt.body);

    // Terminate body
    if (selectedBlock != nullptr) {
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        cfg->linkBlocks(curBlock(), cfg->block(activeControls.back().getLoop().skip));
    }
    selectBlock(blockAfter);

    activeControls.pop_back();
    if (stmt.type == AST_IterStmt::FOR_LOOP)
        variables.decreaseLevel();
}

void IR_Generator::insertJumpStatement(const AST_JumpStmt &stmt) {
    if (stmt.type == AST_JumpStmt::J_RET) {
        auto const &arg = stmt.getExpr();
        if (arg) {
            auto retVal = evalExpr(*arg);
            if (!curFunctionType->ret->equal(*retVal.getType()))
                semanticError(stmt.loc, "Wrong return value type");
            curBlock().setTerminator(IR_ExprTerminator::RET, retVal);
        }
        else {
            if (!curFunctionType->ret->equal(*IR_TypeDirect::getVoid()))
                semanticError(stmt.loc, "Cannot return value in void function");
            curBlock().setTerminator(IR_ExprTerminator::RET);
        }
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_BREAK) {
        if (activeControls.empty())
            semanticError(stmt.loc, "Break keyword outside of loop or switch");
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        cfg->linkBlocks(curBlock(), cfg->block(activeControls.back().getExit()));
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_CONTINUE) {
        auto nearestLoop = getNearestLoop();
        if (!nearestLoop.has_value())
            semanticError(stmt.loc, "Continue keyword outside of loop");
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        cfg->linkBlocks(curBlock(), cfg->block(nearestLoop->skip));
        deselectBlock();
    }
    else if (stmt.type == AST_JumpStmt::J_GOTO) {
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        string_id_t label = stmt.getIdent();
        if (auto it = labels.find(label); it != labels.end())
            cfg->linkBlocks(curBlock(), cfg->block(it->second));
        else
            danglingGotos.emplace_back(curBlock().id, label, stmt.loc);
        deselectBlock();
    }
    else {
        internalError("Wrong jump statement");
    }
}

/** Create new scope and fill block from compound statement */
void IR_Generator::insertCompoundStatement(const AST_CompoundStmt &stmt) {
    if (stmt.body->v.empty())
        return;

    auto &compoundBlock = cfg->createBlock();
    cfg->linkBlocks(curBlock(), compoundBlock);
    curBlock().setTerminator(IR_ExprTerminator::JUMP);
    selectBlock(compoundBlock);
    fillBlock(stmt);

    if (selectedBlock != nullptr) {
        auto &blockAfter = cfg->createBlock();
        cfg->linkBlocks(curBlock(), blockAfter);
        curBlock().setTerminator(IR_ExprTerminator::JUMP);
        selectBlock(blockAfter);
    }
}

void IR_Generator::insertLabeledStatement(const AST_LabeledStmt &stmt) {
    // Create new block
    if (!curBlock().getAllNodes().empty()) {
        IR_Block &nextBlock = cfg->createBlock();
        cfg->linkBlocks(curBlock(), nextBlock);
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
        internalError("Wrong label type");
    }

    insertStatement(*stmt.child);
}
