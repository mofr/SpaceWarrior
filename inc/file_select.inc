function enter_string(imgpath:ansistring):ansistring;
var
  end_:boolean;
  scr_img,img:pimage;
  c:char;
begin
enter_string:='';
loadimagefile(itdetect,imgpath,img,1);
end_:=false;
scr_img:=createimageWH(getmaxx+1,getmaxy+1);
getimage(0,0,scr_img);
repeat
  begin
  if keypressed then
    begin
    c:=readkey;
    case c of
      #27:begin
        end_:=true;
        enter_string:='';
        end;
      #13:begin
        end_:=true;
        end;
      'a'..'z','A'..'Z','0'..'9':enter_string:=enter_string+c;
      #8:enter_string:=copy(enter_string,1,length(enter_string)-1);
      end;
    end;
  composeimagec(scr_img,img,getimagewidth(scr_img)div 2-getimagewidth(img)div 2,getimageheight(scr_img)div 2-getimageheight(img)div 2);
  gf_outtext(scr_img,getmaxx div 2,getmaxy div 2,font3,1,1,enter_string,255);
  putimage(0,0,scr_img);
  end
until end_;
destroyimage(scr_img);
destroyimage(img);
end;

function file_select(dir_:ansistring;type_:integer):ansistring;
const MAX_FILESELECT_BUTTONS=100;
var
  img,imgmask,scr_img,dimg:pimage;
  end_:boolean;
  i,y0:longint;
  fileselect_bt:array[0..MAX_FILESELECT_BUTTONS-1]of Pbutton;
  filename:array[0..MAX_FILESELECT_BUTTONS-1]of ansistring;
  c:char;
  srec:Tsearchrec;
begin
file_select:='';
y0:=getmaxy div 2-getmaxy div 5;
scr_img:=createimageWH(getmaxx+1,getmaxy+1);
dimg:=createimageWH(getmaxx+1,getmaxy+1);
getimage(0,0,dimg);
end_:=false;
case type_ of
  1:begin
    loadimagefile(itdetect,'gfx\other\loadmenu.gif',img,1);
    loadimagefile(itdetect,'gfx\other\loadmenu.gif',imgmask,2);
    end;
  2:begin
    loadimagefile(itdetect,'gfx\other\savemenu.gif',img,1);
    loadimagefile(itdetect,'gfx\other\savemenu.gif',imgmask,2);
    end;
  end;
putimage_wmask(dimg,img,imgmask,getimagewidth(dimg)div 2-getimagewidth(img)div 2,getimageheight(dimg)div 2-getimageheight(img)div 2,255);
//buttons
for i:=0 to MAX_FILESELECT_BUTTONS-1 do
  begin
  fileselect_bt[i]:=nil;
  end;
i:=0;
if type_=2 then
  begin
  fileselect_bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
  fileselect_bt[i]^.img:=gf_text_img('NEW SAVEGAME',font2);
  fileselect_bt[i]^.press_img:=fileselect_bt[i]^.img;
  fileselect_bt[i]^.w:=getmaxx div 4;
  fileselect_bt[i]^.h:=getimageheight(fileselect_bt[i]^.img);
  fileselect_bt[i]^.x:=getmaxx div 2-fileselect_bt[i]^.w div 2;
  fileselect_bt[i]^.y:=y0+i*(font3^.h+4);
  y0:=y0+(font2^.h-font3^.h);
  inc(i);
  end;
if dir_[length(dir_)]<>'\' then dir_:=dir_+'\';
if (findfirst(dir_+'*',faAnyFile,srec)=0) then
repeat
if srec.attr<>faDirectory then
  begin
  fileselect_bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
  fileselect_bt[i]^.img:=gf_text_img(srec.name,font3);
  fileselect_bt[i]^.press_img:=fileselect_bt[i]^.img;
  fileselect_bt[i]^.w:=getmaxx div 4;
  fileselect_bt[i]^.h:=getimageheight(fileselect_bt[i]^.img);
  fileselect_bt[i]^.x:=getmaxx div 2-fileselect_bt[i]^.w div 2;
  fileselect_bt[i]^.y:=y0+i*(font3^.h+4);
  filename[i]:=srec.name;
  inc(i);
  end
until (findnext(srec)<>0)or(i=MAX_FILESELECT_BUTTONS);
//
repeat
  begin
  update_mouse_input;
  for i:=0 to MAX_FILESELECT_BUTTONS-1 do
    if (fileselect_bt[i]<>nil)and(update_button(fileselect_bt[i],pmb,mb,mx,my)=BTA_CLICK) then
      begin
      file_select:=filename[i];
      end_:=true;
      if (type_=2)and(i=0)then file_select:=enter_string('gfx\other\filenamedialog.gif');
      end;
  if keypressed then
    begin
    c:=readkey;
    case c of
      #27:end_:=true;
      end;
    end;
  composeimage(scr_img,dimg,0,0);
  for i:=0 to MAX_FILESELECT_BUTTONS-1 do
    if fileselect_bt[i]<>nil then draw_button(fileselect_bt[i]);
  with cursor do putimage_wmask(scr_img,img^,mask^,mx,my,255);
  putimage(0,0,scr_img);
  end
until end_;
destroyimage(img);
destroyimage(imgmask);
destroyimage(scr_img);
destroyimage(dimg);
end;
