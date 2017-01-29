#!/usr/bin/env python

import json
from json2html import *

def main():
  with open(sys.argv[1]) as inf:
    inputs = inf.read()
  input = json.loads(inputs)
  output = json2html.convert(json = input, table_attributes="class=\"jsontable\"")
  with open(sys.argv[2], 'w') as outf:
    outf.write(output)

main()
