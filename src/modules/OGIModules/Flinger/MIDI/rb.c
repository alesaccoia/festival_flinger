/*********************************************************************************/
/*	   This file includes modifications and/or addditions, made by               */
/*                   David JL Brown during 2003-2004                             */
/*********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSDOS
#define TB
#include <tcl.h> /* for printf def'n */
#endif
#include "rb.h"
#include "list.h"

#define isred(n) (n->s.red)
#define isblack(n) (!isred(n))
#define isleft(n) (n->s.left)
#define isright(n) (!isleft(n))
#define isint(n) (n->s.internal)
#define isext(n) (!isint(n))
#define ishead(n) (n->s.head)
#define isroot(n) (n->s.root)
#define setred(n) n->s.red = 1
#define setblack(n) n->s.red = 0
#define setleft(n) n->s.left = 1
#define setright(n) n->s.left = 0
#define sethead(n) n->s.head = 1
#define setroot(n) n->s.root = 1
#define setint(n) n->s.internal = 1
#define setext(n) n->s.internal = 0
#define setnormal(n) { n->s.root = 0; n ->s.head = 0; }
#define sibling(n) ((isleft(n)) ? n->p.parent->c.child.right \
                                : n->p.parent->c.child.left)
// by DJLB - 'new' replaced by New.
#define mk_new_ext(New, kkkey, vvval) { \
	New = (Rb_node) malloc(sizeof(struct rb_node)); \
	New->v.val = vvval; \
	New->k.key = kkkey; \
	setext(New); \
	setblack(New); \
	setnormal(New); \
}


void mk_new_int(Rb_node l, Rb_node r, Rb_node p, int il)
{ // by DJLB - 'new' replaced by New.
	Rb_node New;

	New = (Rb_node) malloc(sizeof(struct rb_node));
	setint(New);
	setred(New);
	setnormal(New);
	New->c.child.left = l;
	New->c.child.right = r;
	New->p.parent = p;
	New->k.lext = l;
	New->v.rext = r;
	l->p.parent = New;
	r->p.parent = New;
	setleft(l);
	setright(r);
	if (ishead(p)) {
		p->p.root = New;
		setroot(New);
	} else if (il) {
		setleft(New);
		p->c.child.left = New;
	} else {
		setright(New);
		p->c.child.right = New;
	}
	recolor(New);
}


Rb_node lprev(Rb_node n)
{
	if (ishead(n))
		return (n);
	while (!isroot(n)) {
		if (isright(n))
			return (n->p.parent);
		n = n->p.parent;
	}
	return (n->p.parent);
}

Rb_node rprev(Rb_node n)
{
	if (ishead(n))
		return (n);
	while (!isroot(n)) {
		if (isleft(n))
			return (n->p.parent);
		n = n->p.parent;
	}
	return (n->p.parent);
}

Rb_node make_rb()
{
	Rb_node head;

	if ((head = (Rb_node)malloc(sizeof(struct rb_node))) == NULL)
		return (NULL);
	head->c.list.flink = head;
	head->c.list.blink = head;
	head->p.root = head;
	head->k.key = "";
	head->v.val = NULL;
	sethead(head);
	return (head);
}

Rb_node rb_find_key_n(Rb_node n, char *key, int *fnd)
{
	int cmp;

	*fnd = 0;
	if (!ishead(n)) {
		fprintf(stderr, "rb_find_key_n called on non-head 0x%x\n",
		    (int)n);
		exit(1);
	}
	if (n->p.root == n)
		return (n);
	cmp = strcmp(key, n->c.list.blink->k.key);
	if (cmp == 0) {
		*fnd = 1;
		return (n->c.list.blink);
	}
	if (cmp > 0)
		return (n);
	else
		n = n->p.root;
	while (1) {
		if (isext(n))
			return (n);
		cmp = strcmp(key, n->k.lext->k.key);
		if (cmp == 0) {
			*fnd = 1;
			return (n->k.lext);
		}
		if (cmp < 0)
			n = n->c.child.left;
		else
			n = n->c.child.right;
	}
}

Rb_node rb_find_key(Rb_node n, char *key)
{
	int fnd;

	return (rb_find_key_n(n, key, &fnd));
}

Rb_node rb_find_ikey_n(Rb_node n, int ikey, int *fnd)
{
	*fnd = 0;
	if (!ishead(n)) {
		fprintf(stderr, "rb_find_ikey_n called on non-head 0x%x\n",
		    (int)n);
		exit(1);
	}
	if (n->p.root == n)
		return (n);
	if (ikey == n->c.list.blink->k.ikey) {
		*fnd = 1;
		return (n->c.list.blink);
	}
	if (ikey > n->c.list.blink->k.ikey)
		return (n);
	else
		n = n->p.root;
	while (1) {
		if (isext(n))
			return (n);
		if (ikey == n->k.lext->k.ikey) {
			*fnd = 1;
			return (n->k.lext);
		}
		n = (ikey < n->k.lext->k.ikey) ? n->c.child.left :
		    n->c.child.right;
	}
}

Rb_node rb_find_ukey_n(register Rb_node n, unsigned long ukey, int *fnd)
{

	*fnd = 0;
	if (!ishead(n)) {
		fprintf(stderr, "rb_find_ukey_n called on non-head 0x%x\n",
		    (int)n);
		exit(1);
	}
	if (n->p.root == n)
		return (n);
	if (ukey == n->c.list.blink->k.ukey) {
		*fnd = 1;
		return (n->c.list.blink);
	}
	if (ukey > n->c.list.blink->k.ukey)
		return (n);
	else
		n = n->p.root;
	while (1) {
		if (isext(n))
			return (n);
		if (ukey == n->k.lext->k.ukey) {
			*fnd = 1;
			return (n->k.lext);
		}
		n = (ukey < n->k.lext->k.ukey) ? n->c.child.left :
		    n->c.child.right;
	}
}

Rb_node rb_find_ikey(Rb_node n, int ikey)
{
	int fnd;

	return (rb_find_ikey_n(n, ikey, &fnd));
}

Rb_node rb_find_ukey(Rb_node n, unsigned long ukey)
{
	int fnd;

	return (rb_find_ukey_n(n, ukey, &fnd));
}

Rb_node rb_find_gkey_n(Rb_node n, char *key, int(*fxn)(char *key1, char *key2), int *fnd)
{
	int cmp;

	*fnd = 0;
	if (!ishead(n)) {
		fprintf(stderr, "rb_find_key_n called on non-head 0x%x\n",
		    (int)n);
		exit(1);
	}
	if (n->p.root == n)
		return (n);
	cmp = (*fxn)(key, n->c.list.blink->k.key);
	if (cmp == 0) {
		*fnd = 1;
		return (n->c.list.blink);
	}
	if (cmp > 0)
		return (n);
	else
		n = n->p.root;
	while (1) {
		if (isext(n))
			return (n);
		cmp = (*fxn)(key, n->k.lext->k.key);
		if (cmp == 0) {
			*fnd = 1;
			return (n->k.lext);
		}
		if (cmp < 0)
			n = n->c.child.left;
		else
			n = n->c.child.right;
	}
}
//DJLB added param types to (*fxn)()
//Rb_node rb_find_gkey(Rb_node n, char *key, int (*fxn)())
Rb_node rb_find_gkey(Rb_node n, char *key, int (*fxn)(char*, char*))

{
	int fnd;

	return (rb_find_gkey_n(n, key, fxn, &fnd));
}

Rb_node rb_insert_b(Rb_node n, char *key, char *val)
{
	Rb_node newleft, newright, newnode, p;

	if (ishead(n)) {
		if (n->p.root == n) {
			/* Tree is empty */
			mk_new_ext(newnode, key, val);
			insert((List)newnode, (List)n);
			n->p.root = newnode;
			newnode->p.parent = n;
			setroot(newnode);
			return (newnode);
		} else {
			mk_new_ext(newright, key, val);
			insert((List)newright, (List)n);
			newleft = newright->c.list.blink;
			setnormal(newleft);
			mk_new_int(newleft, newright, newleft->p.parent, isleft(newleft));
			p = rprev(newright);
			if (!ishead(p))
				p->k.lext = newright;
			return (newright);
		}
	} else {
		mk_new_ext(newleft, key, val);
		insert((List)newleft, (List)n);
		setnormal(n);
		mk_new_int(newleft, n, n->p.parent, isleft(n));
		p = lprev(newleft);
		if (!ishead(p))
			p->v.rext = newleft;
		return (newleft);
	}
}

void recolor(Rb_node n)
{
	Rb_node p, gp, s;
	int done = 0;

	while (!done) {
		if (isroot(n)) {
			setblack(n);
			return;
		}
		p = n->p.parent;

		if (isblack(p))
			return;

		if (isroot(p)) {
			setblack(p);
			return;
		}
		gp = p->p.parent;
		s = sibling(p);
		if (isred(s)) {
			setblack(p);
			setred(gp);
			setblack(s);
			n = gp;
		} else {
			done = 1;
		}
	}
	/* p's sibling is black, p is red, gp is black */

	if ((isleft(n) == 0) == (isleft(p) == 0)) {
		single_rotate(gp, isleft(n));
		setblack(p);
		setred(gp);
	} else {
		single_rotate(p, isleft(n));
		single_rotate(gp, isleft(n));
		setblack(n);
		setred(gp);
	}
}

void single_rotate(Rb_node y, int l)
{
	int rl, ir;
	Rb_node x, yp;

	/* quiet warning */
	rl = 0;
	ir = isroot(y);
	yp = y->p.parent;
	if (!ir) {
		rl = isleft(y);
	}
	if (l) {
		x = y->c.child.left;
		y->c.child.left = x->c.child.right;
		setleft(y->c.child.left);
		y->c.child.left->p.parent = y;
		x->c.child.right = y;
		setright(y);
	} else {
		x = y->c.child.right;
		y->c.child.right = x->c.child.left;
		setright(y->c.child.right);
		y->c.child.right->p.parent = y;
		x->c.child.left = y;
		setleft(y);
	}

	x->p.parent = yp;
	y->p.parent = x;
	if (ir) {
		yp->p.root = x;
		setnormal(y);
		setroot(x);
	} else {
		if (rl) {
			yp->c.child.left = x;
			setleft(x);
		} else {
			yp->c.child.right = x;
			setright(x);
		}
	}
}

void rb_delete_node(Rb_node n)
{
	Rb_node s, p, gp;
	char ir;

	if (isint(n)) {
		fprintf(stderr, "Cannot delete an internal node: 0x%x\n",
		    (int)n);
		exit(1);
	}
	if (ishead(n)) {
		fprintf(stderr, "Cannot delete the head of an rb_tree: 0x%x\n",
		    (int)n);
		exit(1);
	}
	delete_item((List)n);	/* Delete it from the list */
	p = n->p.parent;	/* The only node */
	if (isroot(n)) {
		p->p.root = p;
		free(n);
		return;
	}
	s = sibling(n);		/* The only node after deletion */
	if (isroot(p)) {
		s->p.parent = p->p.parent;
		s->p.parent->p.root = s;
		setroot(s);
		free(p);
		free(n);
		return;
	}
	gp = p->p.parent;	/* Set parent to sibling */
	s->p.parent = gp;
	if (isleft(p)) {
		gp->c.child.left = s;
		setleft(s);
	} else {
		gp->c.child.right = s;
		setright(s);
	}
	ir = isred(p);
	free(p);
	free(n);

	if (isext(s)) {		/* Update proper rext and lext values */
		p = lprev(s);
		if (!ishead(p))
			p->v.rext = s;
		p = rprev(s);
		if (!ishead(p))
			p->k.lext = s;
	} else if (isblack(s)) {
		fprintf(stderr, "DELETION PROB -- sib is black, internal\n");
		exit(1);
	} else {
		p = lprev(s);
		if (!ishead(p))
			p->v.rext = s->c.child.left;
		p = rprev(s);
		if (!ishead(p))
			p->k.lext = s->c.child.right;
		setblack(s);
		return;
	}

	if (ir)
		return;

	/* Recolor */

	n = s;
	p = n->p.parent;
	s = sibling(n);
	while (isblack(p) && isblack(s) && isint(s) &&
	       isblack(s->c.child.left) && isblack(s->c.child.right)) {
		setred(s);
		n = p;
		if (isroot(n))
			return;
		p = n->p.parent;
		s = sibling(n);
	}

	if (isblack(p) && isred(s)) {	/* Rotation 2.3b */
		single_rotate(p, isright(n));
		setred(p);
		setblack(s);
		s = sibling(n);
	}
	{
		Rb_node x, z;
		char il;

		if (isext(s)) {
			fprintf(stderr,
			    "DELETION ERROR: sibling not internal\n");
			exit(1);
		}
		il = isleft(n);
		x = il ? s->c.child.left : s->c.child.right;
		z = sibling(x);

		if (isred(z)) {	/* Rotation 2.3f */
			single_rotate(p, !il);
			setblack(z);
			if (isred(p))
				setred(s);
			else
				setblack(s);
			setblack(p);
		} else if (isblack(x)) {
			/* Recoloring only (2.3c) */
			if (isred(s) || isblack(p)) {
				fprintf(stderr,
				    "DELETION ERROR: 2.3c not quite right\n");
				exit(1);
			}
			setblack(p);
			setred(s);
			return;
		} else if (isred(p)) {
			/* 2.3d */
			single_rotate(s, il);
			single_rotate(p, !il);
			setblack(x);
			setred(s);
			return;
		} else {
			/* 2.3e */
			single_rotate(s, il);
			single_rotate(p, !il);
			setblack(x);
			return;
		}
	}
}

void rb_print_tree(Rb_node t, int level)
{
	int i;

	if (ishead(t) && t->p.parent == t) {
		printf("tree 0x%x is empty\n",
		    (int)t);
	} else if (ishead(t)) {
		printf("Head: 0x%x.  Root = 0x%x\n", (int)t, (int)t->p.root);
		rb_print_tree(t->p.root, 0);
	} else {
		if (isext(t)) {
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Ext node 0x%x: %c,%c: p=0x%x, k=%s\n", (int)t,
			    isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->p.parent, t->k.key);
		} else {
			rb_print_tree(t->c.child.left, level + 2);
			rb_print_tree(t->c.child.right, level + 2);
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%s,%s)\n",
			    (int)t, isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->c.child.left, (int)t->c.child.right,
			    (int)t->p.parent, t->k.lext->k.key,
			    t->v.rext->k.key);
		}
	}
}

void rb_iprint_tree(Rb_node t, int level)
{
	int i;

	if (ishead(t) && t->p.parent == t) {
		printf("tree 0x%x is empty\n", (int)t);
	} else if (ishead(t)) {
		printf("Head: 0x%x.  Root = 0x%x, < = 0x%x, > = 0x%x\n",
		    (int)t, (int)t->p.root, (int)t->c.list.blink,
		    (int)t->c.list.flink);
		rb_iprint_tree(t->p.root, 0);
	} else {
		if (isext(t)) {
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Ext node 0x%x: %c,%c: p=0x%x, <=0x%x, >=0x%x k=%d\n",
			    (int)t, isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->p.parent, (int)t->c.list.blink,
			    (int)t->c.list.flink, t->k.ikey);
		} else {
			rb_iprint_tree(t->c.child.left, level + 2);
			rb_iprint_tree(t->c.child.right, level + 2);
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%d,%d)\n",
			    (int)t, isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->c.child.left, (int)t->c.child.right,
			    (int)t->p.parent, t->k.lext->k.ikey,
			    t->v.rext->k.ikey);
		}
	}
}

void rb_uprint_tree(Rb_node t, int level)
{
	int             i;
	if (ishead(t) && t->p.parent == t) {
		printf("tree 0x%x is empty\n", (int)t);
	} else if (ishead(t)) {
		printf("Head: 0x%x.  Root = 0x%x, < = 0x%x, > = 0x%x\n",
		    (int)t, (int)t->p.root, (int)t->c.list.blink,
		    (int)t->c.list.flink);
		rb_uprint_tree(t->p.root, 0);
	} else {
		if (isext(t)) {
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Ext node 0x%x: %c,%c: p=0x%x, <=0x%x, >=0x%x k=%lu\n",
			    (int)t, isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->p.parent, (int)t->c.list.blink,
			    (int)t->c.list.flink, t->k.ukey);
		} else {
			rb_uprint_tree(t->c.child.left, level + 2);
			rb_uprint_tree(t->c.child.right, level + 2);
			for (i = 0; i < level; i++)
				putchar(' ');
			printf("Int node 0x%x: %c,%c: l=0x%x, r=0x%x, p=0x%x, lr=(%lu,%lu)\n",
			    (int)t, isred(t) ? 'R' : 'B', isleft(t) ? 'l' : 'r',
			    (int)t->c.child.left, (int)t->c.child.right,
			    (int)t->p.parent, t->k.lext->k.ukey,
			    t->v.rext->k.ukey);
		}
	}
}

int rb_nblack(Rb_node n)
//	Rb_node(n);
{
	int nb;

	if (ishead(n) || isint(n)) {
		fprintf(stderr,
		    "ERROR: rb_nblack called on a non-external node 0x%x\n",
		    (int)n);
		exit(1);
	}
	nb = 0;
	while (!ishead(n)) {
		if (isblack(n))
			nb++;
		n = n->p.parent;
	}
	return (nb);
}

int rb_plength(Rb_node n)
//	Rb_node(n);
{
	int pl;

	if (ishead(n) || isint(n)) {
		fprintf(stderr,
		    "ERROR: rb_plength called on a non-external node 0x%x\n",
		    (int)n);
		exit(1);
	}
	pl = 0;
	while (!ishead(n)) {
		pl++;
		n = n->p.parent;
	}
	return (pl);
}

void
rb_free_tree(Rb_node n)
//	Rb_node(n);
{

	if (!ishead(n)) {
		fprintf(stderr,
		    "ERROR: Rb_free_tree called on a non-head node\n");
		exit(1);
	}
	while (rb_first(n) != nil(n)) {
		rb_delete_node(rb_first(n));
	}
	free(n);
}

char *rb_val(Rb_node n)
//	Rb_node(n);
{
	return (n->v.val);
}
