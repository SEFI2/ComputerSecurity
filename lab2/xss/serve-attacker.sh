#!/bin/bash
gunicorn -w 1 -b example.org:4444 --access-logfile attacker.log attacker:app
