import pickle,sys
ins=pickle.load(open('/tmp/re/ins.pkl','rb'))
for t in sys.argv[1:]:
  print('==',t)
  for a,m,o in ins:
    if t.lower() in o: print(hex(a),m,o)
