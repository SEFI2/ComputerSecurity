#!/bin/bash
scp -P 2222 -i .vagrant/machines/default/virtualbox/private_key  $1 vagrant@localhost:~/.ssh
