#!/bin/bash
gunicorn -w 1 -b example.com:7777 --access-logfile xss.log xss:app
