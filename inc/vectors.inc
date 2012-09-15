type
Tvec=record
    x,y:double;
    end;
Pvec=^Tvec;

function vec_sum(vec1,vec2:Tvec):Tvec;
begin
vec_sum.x:=vec1.x+vec2.x;
vec_sum.y:=vec1.y+vec2.y;
end;

function vec_add(vec:Tvec;val:double):Tvec;
begin
vec_add.x:=vec.x+val;
vec_add.y:=vec.y+val;
end;

function vec_null:Tvec;
begin
vec_null.x:=0;
vec_null.y:=0;
end;

function vec_len(vec:Tvec):double;
begin
vec_len:=sqrt(sqr(vec.x)+sqr(vec.y));
end;

function vec_div(vec:Tvec;val:double):Tvec;
begin
if val<>0 then
  begin
  vec_div.x:=vec.x/val;
  vec_div.y:=vec.y/val;
  end;
end;

function vec_mul(vec:Tvec;val:double):Tvec;
begin
vec_mul.x:=vec.x*val;
vec_mul.y:=vec.y*val;
end;

function vec_unit(vec:Tvec):Tvec;
begin
vec_unit:=vec_div(vec,vec_len(vec));
end;

function vec_norm(vec:Tvec):Tvec;
begin
vec_norm.x:=-vec.y;
vec_norm.y:=vec.x;
end;

function vec_neg(vec:Tvec):Tvec;
begin
vec_neg.x:=-vec.x;
vec_neg.y:=-vec.y;
end;

function vec_eq(vec1,vec2:Tvec):boolean;
begin
vec_eq:=(vec1.x=vec2.x)and(vec1.y=vec2.y);
end;

function vec_newrel(x1,y1,x2,y2:double):Tvec;
begin
vec_newrel.x:=x2-x1;
vec_newrel.y:=y2-y1;
end;

function vec_new(x0,y0:double):Tvec;
begin
vec_new.x:=x0;
vec_new.y:=y0;
end;

procedure vec_draw(img:pimage;vec:Tvec;x0,y0,col:longint);
begin
if img <>nil then
imageline(img,x0,y0,x0+round(vec.x),y0+round(vec.y),col)
else line(x0,y0,x0+round(vec.x),y0+round(vec.y),col);
end;

function vec_angle(vec:Tvec):double;
var l:double;
begin
l:=vec_len(vec);
if l<>0 then vec_angle:=arccos(vec.x/l)else vec_angle:=0;
if (vec.y<0) then vec_angle:=-vec_angle;
end;

function vec_setang(vec:Tvec;ang:double):Tvec;
var len:double;
begin
len:=vec_len(vec);
vec_setang.x:=cos(ang)*len;
vec_setang.y:=sin(ang)*len;
end;

function vec_setangrel(vec:Tvec;ang:double):Tvec;
begin
vec_setangrel.x:=vec.x*cos(ang)-vec.y*sin(ang);
vec_setangrel.y:=vec.x*sin(ang)+vec.y*cos(ang);
end;

function vec_refl(vec,dvec:Tvec):Tvec;
var ang:double;
begin
ang:=2*vec_angle(dvec)-vec_angle(vec);
vec_refl:=vec_setangrel(vec,ang);
end;

function vec_dif(vec1,vec2:Tvec):Tvec;
begin
vec_dif:=vec_sum(vec1,vec_neg(vec2));
end;

function vec_rand:Tvec;
begin
vec_rand.x:=random(1000)/1000;
vec_rand.y:=random(1000)/1000;
vec_rand:=vec_unit(vec_rand);
vec_rand:=vec_setang(vec_rand,random(10));
end;