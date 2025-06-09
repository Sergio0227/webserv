#!/usr/bin/env python3

import os
import sys
import urllib.parse

def parse_get():
	query = os.environ.get('QUERY_STRING', '')
	query = query[1:]
	params = {}
	for pair in query.split('&'):
		if '=' not in pair:
			raise ValueError("Malformed expression")
		key, value = pair.split('=', 1)
		params[key] = value

	return params

def parse_post():
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	post_data = sys.stdin.read(content_length)
	if len(post_data) != content_length:
			raise IOError("Incomplete POST data")
	return post_data

def handle_get(params):

	if params.get('a') is None or params.get('b') is None or params.get('op') is None:
		raise ValueError("Malformed expression")
	try:
		decoded_a = urllib.parse.unquote(params.get('a'))
		a_normalized = decoded_a.replace(',', '.')
		a_num = float(a_normalized)

		decoded_b = urllib.parse.unquote(params.get('b'))
		b_normalized = decoded_b.replace(',', '.')
		b_num = float(b_normalized)

		decoded_op = urllib.parse.unquote(params.get('op'))
	except Exception:
		raise ValueError("Malformed expression")
	res = 0
	if (decoded_op == "-"):
		res = a_num - b_num
	elif (decoded_op == "+"):
		res = a_num + b_num
	elif (decoded_op == "/"):
		if (b_num == 0):
			raise ValueError("Division by zero is undefined")
		res = a_num / b_num
	elif (decoded_op == "*"):
		res = a_num * b_num
	else:
		raise ValueError("Malformed expression")
	print(res)

def handle_post(post_data):
	if not post_data.startswith("text="):
		raise ValueError("Malformed POST data")
	try:
		txt = post_data[5:]
		decoded_txt = urllib.parse.unquote_plus(txt)
		decoded_txt = decoded_txt.replace('\r', '').replace('\n', '')
		print(len(decoded_txt))
	except Exception:
		raise ValueError("Malformed POST data")

method = os.environ.get('REQUEST_METHOD', '')
try:
	if method == 'GET':
		params = parse_get()
		handle_get(params)
	elif method == 'POST':
		post_data = parse_post()
		handle_post(post_data)
except Exception as e:
	print(e)
