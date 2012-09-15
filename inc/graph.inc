type
  Tframe=record
    s1,s2,s3,s4,c1,c2,c3,c4:pimage;
    w1,w2,w3,w4:integer;
    end;
  Pframe=^Tframe;
  Tsep=record
    e1,e2,line:pimage;
    end;

procedure destroy_sep(sep:Tsep);
begin
with sep do
  begin
  destroyimage(e1);
  destroyimage(e2);
  destroyimage(line);
  end;
end;

procedure destroy_frame(var fr:Pframe);
begin
if fr<>nil then
with fr^ do
  begin
  destroyimage(c1);
  destroyimage(c2);
  destroyimage(c3);
  destroyimage(c4);
  destroyimage(s1);
  destroyimage(s2);
  destroyimage(s3);
  destroyimage(s4);
  end;
dispose(fr);
fr:=nil;
end;

procedure load_frame(var fr:Pframe;path:ansistring);
begin
new(fr);
with fr^ do
  begin
  loadimagefile(itdetect,path,c1,1);
  if loadimagefile(itdetect,path,c2,2)=-1 then c2:=nil;
  if loadimagefile(itdetect,path,c3,3)=-1 then c3:=nil;
  if loadimagefile(itdetect,path,c4,4)=-1 then c4:=nil;
  loadimagefile(itdetect,path,s1,5);
  loadimagefile(itdetect,path,s2,6);
  loadimagefile(itdetect,path,s3,7);
  loadimagefile(itdetect,path,s4,8);
  w1:=getimageheight(s1);
  w2:=getimagewidth(s2);
  w3:=getimageheight(s3);
  w4:=getimagewidth(s4);
  end;
end;

procedure load_sep(var sep:Tsep;path:ansistring);
begin
with sep do
  begin
  if loadimagefile(itdetect,path,line,1)=-1 then writeln('Error loading image: "',path,'"');
  if loadimagefile(itdetect,path,e1,2)=-1 then writeln('Error loading image: "',path,'"');
  if loadimagefile(itdetect,path,e2,3)=-1 then writeln('Error loading image: "',path,'"');
  end;
end;

procedure draw_Hsep(dimg:pimage;sep:Tsep;x0,y0,len:integer);
var timg:pimage;
begin
timg:=createimageWH(len,getimageheight(sep.line));
fillimage(timg,getimagetransparencycolor(timg));
for i:=0 to len div getimagewidth(sep.line)+1 do composeimage(timg,sep.line,i*getimagewidth(sep.line),0);
composeimagec(timg,sep.e1,0,0);
composeimagec(timg,sep.e2,len-getimagewidth(sep.e2),0);
composeimagec(dimg,timg,x0,y0-getimageheight(timg)div 2);
destroyimage(timg);
end;

procedure draw_image_alpha(dimg,img:pimage;x,y:longint;alpha:byte);
var t,bg:pimage;
begin
bg:=createimageWH(getimagewidth(img),getimageheight(img));
t:=createimageWH(getimagewidth(img),getimageheight(img));
if dimg<>nil then imagegetimage(dimg,x,y,bg) else getimage(x,y,bg);
copyimage(t,bg);
composeimagec(t,img,0,0);
blendimagealphaimage(t,t,alpha,bg);
if dimg<>nil then
  composeimagec(dimg,t,x,y)
else putimagec(x,y,t);
destroyimage(bg);
destroyimage(t);
end;

procedure draw_bump(dimg,img:pimage;x0,y0:integer);
var timg:pimage;
begin
timg:=createimageWH(getimagewidth(img),getimageheight(img));
getimage(x0,y0,timg);

composeimage(dimg,timg,x0,y0);
destroyimage(timg);
end;

procedure gf_bar(dimg:pimage;x1,y1,sx,sy:longint;rect:boolean;filled:double;col1,col2,alpha:longint);
var timg,bgimg:pimage;
  dx,dy:integer;
begin
dx:=0;dy:=0;
if rect then
  begin
  imagerectangle(dimg,x1,y1,x1+sx-1,y1+sy-1,col1);
  dx:=2;dy:=2;
  end;
timg:=createimageWH(sx-2*dx,sy-2*dy);
fillimage(timg,getimagetransparencycolor(timg));
if filled>0 then imagebar(timg,0,0,round(filled*(sx-2*dx)),sy,col2);
draw_image_alpha(dimg,timg,x1+dx,y1+dy,alpha);
destroyimage(timg);
end;

procedure imagefillimage(dimg,img:pimage);
var i,j:longint;
begin
for i:=0 to getimagewidth(dimg) div getimagewidth(img)+1 do
for j:=0 to getimageheight(dimg) div getimageheight(img)+1 do
  composeimage(dimg,img,i*getimagewidth(img),j*getimageheight(img));
end;

procedure draw_img_bar(dimg,pat:pimage;x1,y1,x2,y2:longint);
var timg:pimage;
begin
timg:=createimageWH(x2-x1,y2-y1);
imagefillimage(timg,pat);
composeimage(dimg,timg,x1,y1);
destroyimage(timg);
end;

procedure putimage_wmask(bg,img,mask:pimage;x,y,alpha:longint);
var timg,tmask:pimage;
begin
alpha:=alpha mod 256;
tmask:=mask;
if alpha<>255 then
  begin
  tmask:=cloneimage(mask);
  imageSSUBcolor(tmask,tmask,rgbcolorrgb(255-alpha,255-alpha,255-alpha));
  end;
timg:=cloneimage(img);
composeimage(timg,bg,-x,-y);
blendimagemaskimage(timg,img,tmask,timg);
if bg<>nil then
    composeimage(bg,timg,x,y)
else putimagec(x,y,timg);
destroyimage(timg);
if tmask<>mask then destroyimage(tmask);
end;

procedure draw_frame(dimg:pimage;main,mask:Pframe;x1,y1,x2,y2:longint);
var frimg:pimage;
    i:longint;
    s1_,s2_,s3_,s4_,c1_,c2_,c3_,c4_:pimage;
    s1_m,s2_m,s3_m,s4_m,c1_m,c2_m,c3_m,c4_m:pimage;
begin
with main^ do
  begin
  s1_:=s1;s2_:=s2;s3_:=s3;s4_:=s4;
  c1_:=c1;c2_:=c2;c3_:=c3;c4_:=c4;
  end;
frimg:=createimageWH(x2-x1+1,y2-y1+1);
fillimage(frimg,getimagetransparencycolor(frimg));
if mask=nil then
begin
if s1_<>nil then
for i:=0 to getimagewidth(frimg)div getimagewidth(s1_)+1 do composeimagec(frimg,s1_,i*getimagewidth(s1_),0);
if s3_<>nil then
for i:=0 to getimagewidth(frimg)div getimagewidth(s3_)+1 do composeimagec(frimg,s3_,i*getimagewidth(s3_),getimageheight(frimg)-getimageheight(s3_));
if s2_<>nil then
for i:=0 to getimageheight(frimg)div getimageheight(s2_)+1 do composeimagec(frimg,s2_,getimagewidth(frimg)-getimagewidth(s2_),i*getimageheight(s2_));
if s4_<>nil then
for i:=0 to getimageheight(frimg)div getimageheight(s4_)+1 do composeimagec(frimg,s4_,0,i*getimageheight(s4_));
if c1_<>nil then composeimagec(frimg,c1_,0,0);
if c2_<>nil then composeimagec(frimg,c2_,getimagewidth(frimg)-getimagewidth(c2_),0);
if c3_<>nil then composeimagec(frimg,c3_,getimagewidth(frimg)-getimagewidth(c3_),getimageheight(frimg)-getimageheight(c3_));
if c4_<>nil then composeimagec(frimg,c4_,0,getimageheight(frimg)-getimageheight(c4_));
end else
begin
with mask^ do
  begin
  s1_m:=s1;s2_m:=s2;s3_m:=s3;s4_m:=s4;
  c1_m:=c1;c2_m:=c2;c3_m:=c3;c4_m:=c4;
  end;
if s1_<>nil then
for i:=0 to getimagewidth(frimg)div getimagewidth(s1_)+1 do putimage_wmask(frimg,s1_,s1_m,i*getimagewidth(s1_),0,255);
if s3_<>nil then
for i:=0 to getimagewidth(frimg)div getimagewidth(s3_)+1 do putimage_wmask(frimg,s3_,s3_m,i*getimagewidth(s3_),getimageheight(frimg)-getimageheight(s3_),255);
if s2_<>nil then
for i:=0 to getimageheight(frimg)div getimageheight(s2_)+1 do putimage_wmask(frimg,s2_,s2_m,getimagewidth(frimg)-getimagewidth(s2_),i*getimageheight(s2_),255);
if s4_<>nil then
for i:=0 to getimageheight(frimg)div getimageheight(s4_)+1 do putimage_wmask(frimg,s4_,s4_m,0,i*getimageheight(s4_),255);
if c1_<>nil then putimage_wmask(frimg,c1_,c1_m,0,0,255);
if c2_<>nil then putimage_wmask(frimg,c2_,c2_m,getimagewidth(frimg)-getimagewidth(c2_),0,255);
if c3_<>nil then putimage_wmask(frimg,c3_,c3_m,getimagewidth(frimg)-getimagewidth(c3_),getimageheight(frimg)-getimageheight(c3_),255);
if c4_<>nil then putimage_wmask(frimg,c4_,c4_m,0,getimageheight(frimg)-getimageheight(c4_),255);
end;
composeimagec(dimg,frimg,x1,y1);
destroyimage(frimg);
end;

type
  Tgf_font=record
    c:array[0..255]of pimage;
    h,dl:integer;
    liters:charset;
    end;
  Pgf_font=^Tgf_font;
var
  gf_coladd,gf_colsub:record
    r,g,b:integer;
    end;
  font1,font2,font3,font4:Pgf_font;

procedure gf_fontload(path:ansistring;var font:Pgf_font);
begin
new(font);
font^.liters:=[];
font^.h:=0;
//digits . / -
for i:=33 to 58 do
    begin
  if loadimagefile(itdetect,path,font^.c[i],i-32)<>-1 then
    begin
    include(font^.liters,chr(i));
    if getimageheight(font^.c[i])>font^.h then font^.h:=getimageheight(font^.c[i]);
    end;
  end;
//Eng alphabet
for i:=65 to 90 do
    begin
  if loadimagefile(itdetect,path,font^.c[i],i-38)<>-1 then
    begin
    include(font^.liters,chr(i));
    if getimageheight(font^.c[i])>font^.h then font^.h:=getimageheight(font^.c[i]);
    end;
  end;
include(font^.liters,#32);
font^.c[32]:=createimageWH(font^.h div 2,font^.h);
fillimage(font^.c[32],getimagetransparencycolor(font^.c[32]));
font^.dl:=font^.h div 10;
end;

function gf_textlen(s:ansistring;font:Pgf_font):longint;
var i:integer;
begin
s:=upcase(s);
gf_textlen:=0;
for i:=1 to length(s) do
  if s[i] in font^.liters then
  inc(gf_textlen,getimagewidth(font^.c[ord(s[i])])+font^.dl);
end;

procedure gf_reset_colors;
begin
with gf_coladd do begin r:=0;g:=0;b:=0;end;
with gf_colsub do begin r:=0;g:=0;b:=0;end;
end;

procedure add_to_byte(var b:byte;val:integer);
var t:integer;
begin
t:=b+val;
if t<0 then t:=0 else
  if t>255 then t:=255;
b:=t;
end;

procedure gf_outtext(dimg:pimage;x0,y0:integer;font:Pgf_font;jx,jy:integer;s:ansistring;alpha:integer);
var timg:pimage;
  i,dx,len,sx,sy:longint;
  r1,g1,b1:byte;
label end_;
begin
if length(s)<1 then goto end_;
s:=upcase(s);
len:=gf_textlen(s,font);
timg:=createimageWH(len,font^.h);
if dimg<>nil then setimagetransparencycolor(timg,getimagetransparencycolor(dimg));
fillimage(timg,getimagetransparencycolor(timg));
dx:=0;
for i:=1 to length(s) do
  if s[i] in font^.liters then
  begin
  composeimagec(timg,font^.c[ord(s[i])],dx,0);
  dx:=dx+gf_textlen(s[i],font);
  end;
x0:=x0-jx*len div 2;
y0:=y0-jy*font^.h div 2;
with gf_coladd do if (r<>0)or(g<>0)or(b<>0)then imageSADDcolor(timg,timg,rgbcolorrgb(r,g,b));
with gf_colsub do if (r<>0)or(g<>0)or(b<>0)then imageSSUBcolor(timg,timg,rgbcolorrgb(r,g,b));
getrgbcolorrgb(getimagetransparencycolor(timg),r1,g1,b1);
with gf_coladd do begin add_to_byte(r1,r);add_to_byte(g1,g);add_to_byte(b1,b);end;
with gf_colsub do begin add_to_byte(r1,-r);add_to_byte(g1,-g);add_to_byte(b1,-b);end;
setimagetransparencycolor(timg,rgbcolorrgb(r1,g1,b1));
draw_image_alpha(dimg,timg,x0,y0,alpha);
destroyimage(timg);
end_:
end;

function gf_text_img(text:ansistring;font:Pgf_font):pimage;
var nimg:pimage;
begin
nimg:=createimageWH(gf_textlen(text,font),font^.h);
fillimage(nimg,TRANSPARENCY_COLOR);
setimagetransparencycolor(nimg,TRANSPARENCY_COLOR);
gf_outtext(nimg,0,0,font,0,0,text,255);
gf_text_img:=nimg;
end;

procedure draw_fps(img:pimage);
var line,i:integer;
begin
line:=1;
if looptime<>0 then
gf_outtext(img,5,5,font4,0,0,'FPS: '+to_str(round(1/looptime)),170);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'Loop time: '+to_str(round(looptime*1000))+' ms:',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'Input: '+to_str(round(lt_input*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'creating image: '+to_str(round(lt_crimg*1000))+' ms',170);inc(line);
for i:=0 to MAX_LAYERS-1 do
//if ltgr_layer[i]>0.001 then
	begin
	gf_outtext(img,25,5+line*(font4^.h+5),font4,0,0,'layer '+to_str(i)+': '+to_str(round(ltgr_layer[i]*1000))+' ms',170);
	inc(line);
	end;
gf_outtext(img,25,5+line*(font4^.h+5),font4,0,0,'other: '+to_str(round(ltgr_other*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'drawing: '+to_str(round(lt_draw*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'bullets: '+to_str(round(lt_bullets*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'effects: '+to_str(round(lt_eff*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'particles: '+to_str(round(lt_part*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'asteroids: '+to_str(round(lt_asteroids*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'containers: '+to_str(round(lt_cont*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'player ship: '+to_str(round(lt_plship*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'ships: '+to_str(round(lt_ships*1000))+' ms',170);inc(line);
gf_outtext(img,5,5+line*(font4^.h+5),font4,0,0,'other: '+to_str(round(lt_other*1000))+' ms',170);inc(line);
end;
