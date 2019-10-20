open Bananas.Typeclasses.Alternative;
open Bananas.Typeclasses.Applicative;
open Bananas.Typeclasses.Functor;
open Bananas.Typeclasses.Monad;
open Bananas.Typeclasses.Monoid;
open Bananas.Typeclasses.Semigroup;
open Bananas.Typeclasses.Traversable;

/* Option as Functor */
module OptionF_: Functor with type t('a) = option('a) = {
  type t('a) = option('a);
  let fmap = f =>
    fun
    | Some(x) => Some(f(x))
    | None => None;
};

module OptionFunctor = FunctorUtils(OptionF_);

/* Option as Applicative */
module OptionA_: Applicative with type t('a) = option('a) = {
  include OptionFunctor;
  let pure = x => Some(x);
  let ap = (f, x) =>
    switch (f, x) {
    | (Some(f), Some(x)) => Some(f(x))
    | _ => None
    };
};

module OptionApplicative = ApplicativeUtils(OptionA_);

/* Option as Alternative */
module OptionA_: Alternative with type t('a) = option('a) = {
  include OptionApplicative;
  let empty = None;
  let (<|>) = (x, k) =>
    switch (x) {
    | None => k
    | x => x
    };
};

module OptionAlternative = AlternativeUtils(OptionA_);

/* Option as Monad */
module OptionM_: Monad with type t('a) = option('a) = {
  include OptionApplicative;
  let bind = (m, f) =>
    switch (m) {
    | None => None
    | Some(x) => f(x)
    };
};

module OptionMonad = MonadUtils(OptionM_);

/* Option as Traversable */
module OptionTraversable =
       (A: Applicative)
       : (
           Traversable with
             type t('a) = option('a) and type Applicative.t('a) = A.t('a)
         ) => {
  type t('a) = option('a);
  module Applicative = A;
  module AppU = ApplicativeUtils(Applicative);
  open AppU;
  let traverse = (f, x) =>
    switch (x) {
    | None => A.pure(None)
    | Some(x) => (a => Some(a)) <$> f(x)
    };
};