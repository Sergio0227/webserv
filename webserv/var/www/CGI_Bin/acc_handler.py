#!/usr/bin/env python3

import os
import sys
import cgi


def emailFound(email, filename):
    try:
        with open(filename, 'r') as file:
            for line in file:
                if email in line:
                    return True
        return False
    except FileNotFoundError:
        print(f"The file {filename} does not exist.")
        return False

def sendConflict():
    


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
    if (emailFound()):
        sendConflict()
    else:
        saveCredentials()
        sendRegistrationSuccess()

def handle_login(params):
    email = params.get('email')
    password = params.get('password')
    print("Logging in:", email)


def handle_upload(params):
    filename = params.get('filename')
    filedata = params.get('filedata')
    print("Uploading:", filename)

def handle_delete(params):
    account = params.get('account')
    print("Deleting account:", account)



image_path = "var/www/data/images"
csv_path = "var/www/data/users.csv"
method = os.environ.get('REQUEST_METHOD', '')
form = cgi.FieldStorage()
action = form.getvalue('action')
if method == 'GET':
    params = parse_get()
    if action == 'register':
        handle_register(params, csv_path)
    elif action == 'login':
        handle_login(params, csv_path)
    else:
        print("Unknown GET action")

elif method == 'POST':
    params = parse_post()
    handle_upload(params, image_path)

elif method == 'DELETE':
    params = parse_get()
    handle_delete(params, csv_path)

else:
    print("Unsupported method:", method)
