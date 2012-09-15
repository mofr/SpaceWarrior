const
  BTS_NORMAL=0;
  BTS_PRESSED=1;
  BTS_DISABLED=2;

  BTT_REGULAR=0;
  BTT_TOGGLE=1;

  BTA_NOTHING=0;
  BTA_ENTER=1;
  BTA_PRESS=2;
  BTA_CLICK=3;

type
Tbutton=record
  x,y,w,h:longint;
  img,img_mask:pimage;
  dimg:pimage;
  state,btt:integer;
  prelighted,mp:boolean;
  //REGULAR
  act_proc:procedure;
  //TOGGLE
  var_toggle:^boolean;
  press_img:pimage;
  //SLIDER
  x_,y_:boolean;
  pmx,pmy,x_min,x_max,y_min,y_max:longint;
  end;
Pbutton=^Tbutton;

function bt_slider_posy(bt:Pbutton):double;
begin
with bt^ do bt_slider_posy:=(y-y_min)/(y_max-y_min-h);
end;

procedure bt_setpos(bt:Pbutton;x,y:longint);
begin
bt^.x:=x;
bt^.y:=y;
end;

function bt_new(btt0:integer;img_path:ansistring;masked:boolean;img_indx,slide:integer;dimg0:pimage):Pbutton;
var nbt:Pbutton;
    i:integer;
begin
new(nbt);
with nbt^ do
begin
  prelighted:=false;
  x:=0;y:=0;
  mp:=false;
  state:=BTS_NORMAL;
  btt:=btt0;
  img_mask:=nil;
  if loadimagefile(itdetect,img_path,img,img_indx)=-1 then img:=nil
  else
    begin
    w:=getimagewidth(img);
    h:=getimageheight(img);
    if masked then loadimagefile(itdetect,img_path,img_mask,img_indx+1)
    end;
  press_img:=img;
  dimg:=dimg0;
  act_proc:=nil;
  pmx:=0;pmy:=0;
  case slide of
    0:begin x_:=false;y_:=false;end;
    1:begin x_:=true;y_:=false;end;
    2:begin x_:=false;y_:=true;end;
    end;
  end;
bt_new:=nbt;
end;

procedure destroy_button(var bt:Pbutton);
begin
destroyimage(bt^.img);
dispose(bt);
bt:=nil;
end;

function inbutton(bt:Pbutton;x,y:longint):boolean;
begin
inbutton:=((x>bt^.x)and(x<bt^.x+bt^.w))and((y>bt^.y)and(y<bt^.y+bt^.h));
end;

procedure draw_button(bt:Pbutton);
var
  i,t:longint;
  timg:pimage;
begin
with bt^ do
if img<>nil then
  begin
  if state=BTS_PRESSED then timg:=cloneimage(press_img) else timg:=cloneimage(img);
  if prelighted then imageSADDcolor(timg,timg,rgbcolorrgb(30,30,30));
  if prelighted and(getimagetransparencycolor(timg)=rgbcolorrgb(0,0,0))then setimagetransparencycolor(timg,rgbcolorrgb(30,30,30));
  if img_mask<>nil then putimage_wmask(dimg,timg,img_mask,x+w div 2-getimagewidth(timg)div 2,y+h div 2-getimageheight(timg)div 2,255)else
  composeimagec(dimg,timg,x+w div 2-getimagewidth(timg)div 2,y+h div 2-getimageheight(timg)div 2);
  destroyimage(timg);
  end;
end;

function update_button(bt:Pbutton;pmb,mb,mx,my:integer):integer;
{returns last action}
var ac:integer;
label end_;
begin
ac:=BTA_NOTHING;
if bt^.state=BTS_DISABLED then 
  begin
  bt^.prelighted:=false;
  goto end_;
  end;
bt^.prelighted:=inbutton(bt,mx,my);
if bt^.prelighted and(mb=1)and(pmb=0) then bt^.mp:=true;
if bt^.mp and(mb=1)and(pmb=0) then
  begin
  bt^.pmx:=mx;
  bt^.pmy:=my;
  end;
case bt^.btt of
  BTT_REGULAR:if bt^.prelighted and bt^.mp then
    begin
    if (mb=1)then bt^.state:=BTS_PRESSED;
    if (mb=0)and(pmb=1)then ac:=BTA_CLICK;
    end
    else bt^.state:=BTS_NORMAL;
  BTT_TOGGLE:if bt^.mp then case bt^.state of
    BTS_PRESSED:if bt^.prelighted and(mb=0)and(pmb=1)then
        begin
      bt^.state:=BTS_NORMAL;
      bt^.var_toggle^:=false;
      end;
    BTS_NORMAL:if bt^.prelighted and(mb=0)and(pmb=1)then
        begin
      bt^.state:=BTS_PRESSED;
      bt^.var_toggle^:=true;
      end;
    end;
  end;

if mb=0 then bt^.mp:=false;
if bt^.mp then
with bt^ do
  begin
  if x_ then
    begin
    if (mx>=x_min)and(mx<=x_max)then x:=mx-pmx+x;
    pmx:=mx;
    if x<x_min then x:=x_min;
    if x+w>x_max then x:=x_max-w;
    end;
  if y_ then
    begin
    if (my>=bt^.y_min)and(my<=bt^.y_max)then y:=my-pmy+y;
    pmy:=my;
    if y<y_min then y:=y_min;
    if y+h>y_max then y:=y_max-h;
    end;
  end;

case bt^.btt of
  BTT_REGULAR:if (ac<>BTA_NOTHING)and(bt^.act_proc<>nil) then
    begin
    add_sound('sfx\click.wav',false,false,0,0);
    bt^.act_proc();
    end;
  BTT_TOGGLE:if bt^.var_toggle^ then bt^.state:=BTS_PRESSED else bt^.state:=BTS_NORMAL;
  end;
end_:
update_button:=ac;
end;
