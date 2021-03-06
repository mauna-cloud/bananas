/* List.re */

open Base;

open Typeclasses.Alternative;
open Typeclasses.Applicative;
open Typeclasses.Functor;
open Typeclasses.Monad;
open Typeclasses.Monoid;
open Typeclasses.Semigroup;
open Typeclasses.Traversable;

/* List as Functor */
module ListF_: Functor with type t('a) = list('a) = {
  type t('a) = list('a);
  let fmap = (f, ls) => List.map(ls, f);

  let (===) = (a: t('a), b: t('a)) => {
    let are_equal: t(bool) = List.mapi(
      a,
      ~f=(idx, elem_a: 'a) => switch (List.nth(b, idx)) {
        | Some(elem_b) => elem_b === elem_a
        | None => false
      }
    );

    let result = List.fold(are_equal, ~init=true, ~f=(&&));
    result;
  };
};

module ListFunctor = FunctorUtils(ListF_);

/* List as Applicative */
module ListA_: Applicative with type t('a) = List.t('a) = {
  include ListFunctor;
  let pure = x => [x];
  let ap = (fs, xs) => fmap(f => fmap(x => f(x), xs), fs) |> List.concat;
};

module ListApplicative = ApplicativeUtils(ListA_);

/* List as Alternative */
module ListAlt_: Alternative with type t('a) = list('a) = {
  include ListApplicative;
  let empty = [];
  let (<|>) = (@);
};

module ListAlternative = AlternativeUtils(ListAlt_);

/* List as Monad */

module ListM_: Monad with type t('a) = list('a) = {
  include ListApplicative;
  let bind = (m, f) => 
    List.fold_right(m, ~f = (x, y) => List.append(f(x), y), ~init = []);
};

module ListMonad = MonadUtils(ListM_);

// Generic type
module type GenericTypeConstuctor = {type t;};

/* List as Semigroup */
module ListSemigroup =
       (T: GenericTypeConstuctor)
       : (Semigroup with type t = list(T.t)) => {
  type t = list(T.t);
  let append = (@);
};

/* List as Monoid */
module ListMonoid_ = (T: GenericTypeConstuctor) : (Monoid with type t = list(T.t)) => {
  include
    ListSemigroup(
      {
        type t = T.t;
      },
    );
  let empty = [];
};

module ListMonoid = (T: GenericTypeConstuctor) =>
  MonoidUtils(
    (
      ListMonoid_(
        {
          type t = T.t;
        },
      )
    ),
  );

/* List as Traversable */  
module ListTraversable =
       (A: Applicative)
       : (
           Traversable with
             type t('a) = list('a) and type Applicative.t('a) = A.t('a)
         ) => {
  type t('a) = list('a);
  module Applicative = A;
  module AppU = ApplicativeUtils(Applicative);
  open AppU;
  let rec traverse = (f, xs) =>
    switch (xs) {
    | [] => A.pure([])
    | [x, ...xs] => ((y, ys) => [y, ...ys]) <$> f(x) <*> traverse(f, xs)
    };
};
