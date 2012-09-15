var
	options_quit:boolean;
	scr_img_options,options_img:pimage;
	options_prev_loop:procedure;
	options_bt:array[0..4]of Pbutton;
	bt_setcolors16,bt_setcolors32:Pbutton;

procedure options_callback1;
begin
kol_colors:=16;
end;
procedure options_callback2;
begin
kol_colors:=32;
end;

procedure options_init(prevproc:Tprocedure);
var i:integer;
begin
loadimagefile(itdetect,'gfx\other\options.gif',options_img,0);
setimagetransparencycolor(options_img,rgbcolorrgb(255,0,0));
options_prev_loop:=prevproc;
scr_img_options:=createimageWH(getmaxx+1,getmaxy+1);
options_quit:=false;
i:=0;
options_bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img_options);
options_bt[i]^.act_proc:=@options_callback1;
options_bt[i]^.img:=gf_text_img('16',font3);
options_bt[i]^.press_img:=options_bt[i]^.img;
options_bt[i]^.w:=getimagewidth(options_bt[i]^.img);
options_bt[i]^.h:=getimageheight(options_bt[i]^.img);
bt_setcolors16:=options_bt[i];
inc(i);
options_bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img_options);
options_bt[i]^.act_proc:=@options_callback2;
options_bt[i]^.img:=gf_text_img('32',font3);
options_bt[i]^.press_img:=options_bt[i]^.img;
options_bt[i]^.w:=getimagewidth(options_bt[i]^.img);
options_bt[i]^.h:=getimageheight(options_bt[i]^.img);
bt_setcolors32:=options_bt[i];
inc(i);
end;

procedure options_quit_loop;
begin
destroyimage(options_img);
destroyimage(scr_img_options);
save_main_settings('config.cfg');
end;

procedure create_scr_img_options(var dimg:pimage;dr_cur:boolean);
var
	x1,x2,y0,dy,d:integer;
begin
composeimage(dimg,scr_img,0,0);
composeimagec(dimg,options_img,getmaxx div 2-getimagewidth(options_img)div 2,getmaxy div 2-getimageheight(options_img)div 2);
x1:=getmaxx div 2-getmaxx div 6;
x2:=getmaxx div 2;
y0:=getmaxy div 2-getmaxy div 6;
dy:=0;
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Game speed',255);inc(dy,font3^.h+3);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Sound volume',255);inc(dy,font3^.h+3);
gf_outtext(dimg,x1,y0+dy,font3,0,0,'Colors',255);
bt_setpos(bt_setcolors16,x2,y0+dy+font3^.h div 2-bt_setcolors16^.h div 2);
bt_setpos(bt_setcolors32,bt_setcolors16^.x+bt_setcolors16^.w+10,bt_setcolors16^.y);
inc(dy,font3^.h+3);

draw_button(bt_setcolors16);
draw_button(bt_setcolors32);
case kol_colors of
	32:draw_frame(dimg,smfr,nil,bt_setcolors32^.x-5,bt_setcolors32^.y-5,bt_setcolors32^.x+bt_setcolors32^.w+4,bt_setcolors32^.y+bt_setcolors32^.h+4);
	16:draw_frame(dimg,smfr,nil,bt_setcolors16^.x-5,bt_setcolors16^.y-5,bt_setcolors16^.x+bt_setcolors16^.w+4,bt_setcolors16^.y+bt_setcolors16^.h+4);
	end;
if dr_cur then
with cursor do
  putimage_wmask(scr_img_options,img^,mask^,mx,my,255);
end;

procedure options_loop;
var c:char;
begin
update_mouse_input;
if keypressed then
  begin
  c:=readkey;
  case c of
    #27:options_quit:=true;
    end;
  end;
create_scr_img_options(scr_img_options,true);
putimage(0,0,scr_img_options);
if options_quit then
  begin
  options_quit_loop;
  main_loop_proc:=options_prev_loop;
  end;
update_mouse_input;
for i:=0 to 1 do update_button(options_bt[i],pmb,mb,mx,my);
end;

procedure show_options;
begin
options_init(main_loop_proc);
main_loop_proc:=@options_loop;
end;
