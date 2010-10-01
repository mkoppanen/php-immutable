<?php

$immutable = new Immutable(array("x" => 1, "y" => 2, "z" => 3, 11 => "hello"));

$immutable->x = "something else";
$tmp =& $immutable->x;

$tmp = "foobar";

var_dump($immutable);

