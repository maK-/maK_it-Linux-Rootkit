#!/bin/bash
kill `ps -ef | grep maK_it_shell | grep -v grep | awk '{print $2}'`
