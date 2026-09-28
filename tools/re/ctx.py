import pickle,sys,bisect
ins=pickle.load(open('/tmp/re/ins.pkl','rb'));addrs=[i[0] for i in ins]
for t in sys.argv[2:]:
  a=int(t,16);k=bisect.bisect_left(addrs,a);n=int(sys.argv[1])
  print('=====',t)
  for x in ins[max(0,k-n):k+n]: print(hex(x[0]),x[1],x[2])
