//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

struct node {
    char *key;
	int val;
	struct node *parent, *left, *right;
	int balance;
};

struct node* Minimum(struct node *t) {
	if (t == NULL)
		return NULL;
	else {
        struct node *x = t;
		while (x->left != NULL)
			x = x->left;
		return x;
	}
}

struct node* Succ(struct node *x) {
	if (x->right != NULL)
		return Minimum(x->right);
	else {
        struct node *y = x->parent;
		while (y != NULL && x == y->right) {
				x = y;
				y = y->parent;
		}
		return y;
	}
}

struct node* Descend(struct node *t, char *key) {
    struct node *x = t;
	while (x != NULL && strcmp(x->key, key) != 0) {
		if (strcmp(x->key, key) > 0)
			x = x->left;
		else
			x = x->right;
	}
	return x;
}

struct node* Lookup(struct node *t, char *key) {
	return Descend(t, key);
}

struct node* Insert(struct node **_t, char *key, int val) {
    struct node *t = *_t;
    struct node *y = (struct node*)malloc(sizeof(struct node));
	y->key = key;
	y->val = val;
	y->parent = NULL;
	y->right = NULL;
	y->left = NULL;
	y->balance = 0;
	if (t == NULL)
		t = y;
	else {
        struct node *x = t;
		while (1) {
			if (strcmp(x->key, key) > 0) {
				if (x->left == NULL) {
					x->left = y;
					y->parent = x;
					break;
				}
				x = x->left;
			}
			else {
				if (x->right == NULL) {
					x->right = y;
					y->parent = x;
					break;
				}
				x = x->right;
			}
		}
	}
	*_t = t;
	return y;
}

void ReplaceNode(struct node **_t, struct node *x, struct node *y) {
    struct node *t = *_t;
	if (x == t) {
		t = y;
		if (y != NULL)
			y->parent = NULL;
	}
	else {
        struct node *p = x->parent;
		if (y != NULL)
			y->parent = p;
		if (p->left == x)
			p->left = y;
		else
			p->right = y;
	}
	*_t = t;
}

void RotateLeft(struct node **t, struct node *x) {
    struct node *y = x->right;
	ReplaceNode(t, x, y);
    struct node *b = y->left;
	if (b != NULL)
		b->parent = x;
	x->right = b;
	x->parent = y;
	y->left = x;
	//
	--x->balance;
	if (y->balance > 0)
		x->balance -= y->balance;
	--y->balance;
	if (x->balance < 0)
		y->balance += x->balance;
}

void RotateRight(struct node **t, struct node *x) {
    struct node *y = x->left;
	ReplaceNode(t, x, y);
    struct node *b = y->right;
	if (b != NULL)
		b->parent = x;
	x->left = b;
	x->parent = y;
	y->right = x;
	//
	++x->balance;
	if (y->balance < 0)
		x->balance -= y->balance;
	++y->balance;
	if (x->balance > 0)
		y->balance += x->balance;
}

void InsertAVL(struct node **t, char *key, int val) {
    struct node *a = Insert(t, key, val);
	a->balance = 0;
	while (1) {
        struct node *x = a->parent;
		if (x == NULL)
			break;
		if (a == x->left) {
			x->balance--;
			if (x->balance == 0)
				break;
			if (x->balance == -2) {
				if (a->balance == 1)
					RotateLeft(t, a);
				RotateRight(t, x);
				break;
			}
		}
		else {
			x->balance++;
			if (x->balance == 0)
				break;
			if (x->balance == 2) {
				if (a->balance == -1)
					RotateRight(t, a);
				RotateLeft(t, x);
				break;
			}
		}
		a = x;
	}
}

int ScanConst(char **str) {
	int ret = 0;
	while (**str >= "0" && **str <= "9") {
		ret = (ret * 10) + (**str - "0");
		++*str; 
	}
	return ret;
}

int ScanIdent(char **str, struct node **tree, int *identCount) {
	char ident[32];
	int i = 0;
	while (	**str != "\0" &&
			**str != " " &&
			**str != "+" &&
			**str != "-" &&
			**str != "*" &&
			**str != "/" &&
			**str != "(" &&
			**str != ")") {
		ident[i] = **str;
		++i;
		++*str;	
	}
	ident[i] = "\0";
    struct node *t = Lookup(*tree, ident);
	if (t == NULL) {
		char *st = (char*)malloc((strlen(ident) + 1) * sizeof(char));
		strcpy(st, ident);
		InsertAVL(tree, st, *identCount);
		return (*identCount)++;
	}
	else {
		return t->val;
	}
}

void freeRec(struct node *tree) {
	if (tree->left != NULL)
		freeRec(tree->left);
	if (tree->right != NULL)
		freeRec(tree->right);
	free(tree->key);
	free(tree);
}

int main(int argc, char **argv) {
	int n;
	scanf("%i ", &n);
	char expr[n + 1];
	gets(expr);
	char *t = expr;
    struct node *tree = NULL;
	int identCount = 0;
	while (*t != "\0") {
		if (*t == " ")
			++t;
		else if (*t == "+") {
			printf("SPEC 0\n");
			++t;
		}
		else if (*t == "-") {
			printf("SPEC 1\n");
			++t;
		}
		else if (*t == "*") {
			printf("SPEC 2\n");
			++t;
		}
		else if (*t == "/") {
			printf("SPEC 3\n");
			++t;
		}
		else if (*t == "(") {
			printf("SPEC 4\n");
			++t;
		}
		else if (*t == ")") {
			printf("SPEC 5\n");
			++t;
		}
		else if  (*t >= "0" && *t <= "9")
			printf("CONST %i\n", ScanConst(&t));
		else {
			printf("IDENT %i\n", ScanIdent(&t, &tree, &identCount));
		}
	}
	if (tree != NULL)
		freeRec(tree);
	return 0;
}
