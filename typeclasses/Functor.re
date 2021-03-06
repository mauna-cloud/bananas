module type Functor = {
  type t('a); 
  let fmap: ('a => 'b, t('a)) => t('b);
  let (===): (t('a), t('a)) => bool;
};

module FunctorUtils = (F: Functor) => {
  open Library.Util;
  include F;
  let (<$>) = (f, x) => fmap(f, x);
  let (<&>) = (x, f) => flip(fmap, x, f);
  let (<$) = (r, x) => fmap(const(r), x);
  let ($>) = (r, x) => flip((<$), r, x);
  let void = (f, x) => fmap(x => ignore(f(x)), x);
};

module FunctorLaws = (F: Functor) => {
  open Library.Util;
  open F;

  let idLaw = x => fmap(id, x) == x;
  let composeLaw = (f, g, xs) => fmap(f <<< g, xs) == fmap(f, fmap(g, xs));
};
