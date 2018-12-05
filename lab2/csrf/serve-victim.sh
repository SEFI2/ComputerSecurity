#!/bin/bash
gunicorn -w 1 -b example.net:8888 --access-logfile victim.log victim:app
