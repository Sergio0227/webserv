#!/usr/bin/env python3

import os
import sys

def parse_get():
    query = os.environ.get('QUERY_STRING', '')
    params = dict(pair.split('=') for pair in query.split('&') if '=' in pair) #for pair in ... if '=' in pair iterates over pair in a=b&c=d&
    return params

def parse_post():
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
    params = dict(pair.split('=') for pair in post_data.split('&') if '=' in pair)
    return params

def handle_register(params):
    email = params.get('email')
    password = params.get('password')
    print("Registering:", email)
    # Add your logic here

def handle_login(params):
    email = params.get('email')
    password = params.get('password')
    print("Logging in:", email)
    # Add your logic here

def handle_upload(params):
    filename = params.get('filename')
    filedata = params.get('filedata')
    print("Uploading:", filename)
    # Add your logic here

def handle_delete(params):
    account = params.get('account')
    print("Deleting account:", account)
    # Add your logic here

method = os.environ.get('REQUEST_METHOD', '')
if method == 'GET':
    params = parse_get()
    action = params.get('action')
    if action == 'register':
        handle_register(params)
    elif action == 'login':
        handle_login(params)
    else:
        print("Unknown GET action")

elif method == 'POST':
    params = parse_post()
    handle_upload(params)

elif method == 'DELETE':
    params = parse_get()
    handle_delete(params)

else:
    print("Unsupported method:", method)
