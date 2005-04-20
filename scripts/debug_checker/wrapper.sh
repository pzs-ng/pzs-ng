#!/bin/sh
ulimit -c unlimited
binary="${0%%.sh}"
exec $binary "$@"

