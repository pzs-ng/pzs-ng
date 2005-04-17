#!/bin/sh

$1 -u -w | cut -c 6-18,54-62 | grep -ve "----------------------" | grep -ve "Username.*Mbyt"
