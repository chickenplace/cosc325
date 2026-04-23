male(jake).
male(judson).
male(eric).
male(duane).
male(linton).
male(john).
male(scott).
male(chris).

female(amy).
female(carla).
female(mary).

parent(eric, jake).
parent(eric, judson).
parent(amy, jake).
parent(amy, judson).
parent(carla, amy).
parent(duane, amy).
parent(carla, scott).
parent(duane, scott).
parent(mary, eric).
parent(linton, eric).
parent(john, eric).
parent(mary, chris).
parent(linton, chris).
parent(john, chris).

father(X, Y) :- male(X), parent(X, Y).
mother(X, Y) :- female(X), parent(X, Y).
child(X, Y) :- parent(Y, X).
son(X, Y) :- child(X, Y), male(X).
daughter(X, Y) :- child(X, Y), female(X).
grandparent(X, Y) :- parent(X, Z), parent(Z, Y).
grandfather(X, Y) :- grandparent(X, Y), male(X).
grandmother(X, Y) :- grandparent(X, Y), female(X).

aunt(X, Y) :- parent(Z, Y), fullsibling(X, Z), female(X).
aunt(X, Y) :- parent(Z, Y), halfsibling(X, Z), female(X).
uncle(X, Y) :- parent(Z, Y), fullsibling(X, Z), male(X).
uncle(X, Y) :- parent(Z, Y), halfsibling(X, Z), male(X).

fullsibling(X, Y) :- mother(M, X), mother(M, Y), father(F, X), father(F, Y), X \= Y.
halfsibling(X, Y) :- mother(M, X), mother(M, Y), father(F, X), father(G, Y), F \= G.
halfsibling(X, Y) :- mother(M, X), mother(N, Y), father(F, X), father(F, Y), M \= N.


maxl_no_nest(H,[H]).
maxl_no_nest(M,[M|T]) :- maxl_no_nest(TM,T), M>TM.
maxl_no_nest(TM,[M|T]) :- maxl_no_nest(TM,T), M=<TM.

maxl_ignore(H,[H]) :- \+ is_list(H).

%reverse(R, L) means R is the reversed version of L
reverse([],[]). %base case empty list (special and will not be hit during recursion)
reverse([H],[H]) :- !. %alt base case for one iem list
reverse(R, [H|T]) :- reverse(TR, T), append(TR, [H], R).
