#!/usr/bin/env python3

import os
import sys
import urllib.parse

def parse_get():
	query = os.environ.get('QUERY_STRING', '')
	query = query[1:]
	params = dict(pair.split('=') for pair in query.split('&') if '=' in pair) #for pair in ... if '=' in pair iterates over pair in a=b&c=d&
	return params

def parse_post():
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	post_data = sys.stdin.read(content_length)
	return post_data

def handle_get(params):
	error = False
	decoded_a = urllib.parse.unquote(params.get('a'))
	a_normalized = decoded_a.replace(',', '.')
	try:
		a_num = float(a_normalized)
	except ValueError:
		error = True

	decoded_op = urllib.parse.unquote(params.get('op'))
	decoded_b = urllib.parse.unquote(params.get('b'))
	b_normalized = decoded_b.replace(',', '.')
	try:
		b_num = float(b_normalized)
	except ValueError:
		error = True
	if (error):
		print("NaN")
		return
	res = 0
	if (decoded_op == "-"):
		res = a_num - b_num
	elif (decoded_op == "+"):
		res = a_num + b_num
	elif (decoded_op == "/"):
		if (b_num == 0):
			print("Division by zero is undefined")
			return
		res = a_num / b_num
	elif (decoded_op == "*"):
		res = a_num * b_num
	print(res)

def handle_post(post_data):
	txt = post_data[5:]
	decoded_txt = urllib.parse.unquote_plus(txt)
	decoded_txt = decoded_txt.replace('\r', '').replace('\n', '')
	print(len(decoded_txt))

method = os.environ.get('REQUEST_METHOD', '')
if method == 'GET':
    params = parse_get()
    handle_get(params)
elif method == 'POST':
	post_data = parse_post()
	handle_post(post_data)
