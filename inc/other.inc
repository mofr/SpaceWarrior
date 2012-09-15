procedure incf(var val:double;inc:double);
begin
val:=val+inc;
end;

function rnd(prob:float):boolean;
begin
if prob>100 then prob:=100;
if prob<0 then prob:=0;
rnd:=random(round(1000*100/prob))div 1000=0;
end;

function str_between(s:ansistring;ch1,ch2:char):ansistring;
begin
str_between:=copy(s,1,pos(ch2,s)-1);
str_between:=copy(str_between,pos(ch1,s)+1,length(str_between)-pos(ch1,str_between));
end;

function file_exist(path:ansistring):boolean;
var f:text;
begin
assign(f,path);
{$i-}reset(f);{$i+}
file_exist:=ioresult=0;
if file_exist then close(f);
end;

function to_str(val:longint):ansistring;
begin
str(val,to_str);
end;

function getval(s:ansistring;c:char):ansistring;
begin
getval:=copy(s,pos(c,s)+1,length(s)-pos(c,s));
getval:=copy(getval,1,pos(c,getval)-1);
end;

function sign(val:double):integer;
begin
if val<0 then sign:=-1 else
if val>0 then sign:=1 else sign:=0;
end;

procedure swap_pointer(var p1,p2:pointer);
var t:pointer;
begin
t:=p1;
p1:=p2;
p2:=t;
end;

procedure swap_int(var a,b:integer);
var t:integer;
begin
t:=a;a:=b;b:=t;
end;

function rect_cross(x1,y1,x2,y2{},x3,y3,x4,y4:integer):boolean;
  function in_(val,v1,v2:integer):boolean;
    begin
    if v1>v2 then swap_int(v1,v2);
    in_:=(val>=v1)and(val<=v2);
    end;
begin
if x1>x2 then swap_int(x1,x2);
if x3>x4 then swap_int(x3,x4);
if y1<y2 then swap_int(y1,y2);
if y3<y4 then swap_int(y3,y4);
rect_cross:=(in_(x1,x3,x4)and in_(y2,y3,y4))or
  (in_(x2,x3,x4)and in_(y1,y3,y4))or
  (in_(x3,x1,x2)and in_(y4,y1,y2))or
  (in_(x4,x1,x2)and in_(y3,y1,y2))or
  (in_(x1,x3,x4)and in_(y1,y3,y4))or
  (in_(x2,x3,x4)and in_(y2,y3,y4))or
  (in_(x3,x1,x2)and in_(y3,y1,y2))or
  (in_(x4,x1,x2)and in_(y4,y1,y2));
end;

function point_in_rect(px,py,x1,y1,x2,y2:integer):boolean;
begin
if x1>x2 then swap_int(x1,x2);
if y1<y2 then swap_int(y1,y2);
point_in_rect:=(px>=x1)and(px<=x2)and(py>=y1)and(py<=y2);
end;


function kol_param(dst:ansistring):integer;
begin
kol_param:=1;
while (pos(',',dst)<>0)do
  begin
  dst:=copy(dst,pos(',',dst)+1,length(dst)-pos(',',dst));
  inc(kol_param);
  end;
end;

function get_param(dst:ansistring;indx:integer):ansistring;
begin
dst:=dst+',';
while (indx>1)and(length(dst)>0) do
  begin
  dst:=copy(dst,pos(',',dst)+1,length(dst)-pos(',',dst));
  dec(indx);
  end;
get_param:=copy(dst,1,pos(',',dst)-1);
end;

function lines_crossed(x11,y11,x12,y12,x21,y21,x22,y22:real;rx,ry:Plongint):boolean;
var k1,k2,b1,b2,t:double;
    x,y:double;
    h1,h2:boolean;
begin
lines_crossed:=false;

h1:=x12-x11=0;
h2:=x22-x21=0;
if not h1 then k1:=(y12-y11)/(x12-x11) else k1:=0;
if not h2 then k2:=(y22-y21)/(x22-x21) else k2:=0;

if k1<>k2 then
  begin
  if h1 then
    begin
    b2:=y21-k2*x21;
    x:=x11;
    y:=k2*x+b2;
    end
  else if h2 then
    begin
    b1:=y11-k1*x11;
    x:=x21;
    y:=k1*x+b1;
    end
  else
    begin
    b1:=y11-k1*x11;
    b2:=y21-k2*x21;
    x:=(b2-b1)/(k1-k2);
    y:=k1*x+b1;
    end;

  if x11>x12 then begin t:=x11;x11:=x12;x12:=t;end;
  if x21>x22 then begin t:=x21;x21:=x22;x22:=t;end;
  if y11>y12 then begin t:=y11;y11:=y12;y12:=t;end;
  if y21>y22 then begin t:=y21;y21:=y22;y22:=t;end;
  lines_crossed:=(x>=x11)and(x<=x12)and(x>=x21)and(x<=x22)
    and(y>=y11)and(y<=y12)and(y>=y21)and(y<=y22);
  end;
if lines_crossed then
  begin
  if rx<>nil then rx^:=round(x);
  if ry<>nil then ry^:=round(y);
  end;
end;

procedure make_screenshot;
var scr:pimage;
  indx:integer;
begin
scr:=createimageWH(getmaxx+1,getmaxy+1);
getimage(0,0,scr);
indx:=1;
while file_exist('screenshots\screenshot'+to_str(indx)+'.tga')and(indx<high(indx)) do inc(indx);
saveimageTGA('screenshots\screenshot'+to_str(indx)+'.tga',scr);
destroyimage(scr);
end;
