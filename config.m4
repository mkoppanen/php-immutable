PHP_ARG_ENABLE(immutable,      [whether to enable immutable object support],
[ --enable-immutable[=DIR]      Enable immutable object support])

if test "$PHP_IMMUTABLE" != "no"; then
  PHP_NEW_EXTENSION(immutable, immutable.c, $ext_shared)
fi

