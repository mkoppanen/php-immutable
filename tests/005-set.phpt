--TEST--
Test setting properties
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

$immutable = new Immutable();
$immutable->x = "123";
$immutable->x = "456";

var_dump ($immutable->x);

echo "OK\n";

?>
--EXPECTF--
string(3) "123"
OK