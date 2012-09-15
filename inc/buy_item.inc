var buying_item:PPitem;
  dialog_x0,dialog_sx,dialog_y0,dialog_sy:integer;
  item_cost:longint;
  buying_scr_img,dialog_img:pimage;
  b1,b2:Pbutton;
  enough_money:boolean;

procedure buy_item_loop;forward;
procedure buy_item_init(itm:PPitem;x0,y0:integer);forward;
procedure buy_item_quit;forward;

procedure buy_item_buy;
begin
inventory_add_item(buying_item);
warehouse_remove_item(buying_item);
ppitem_array_compress(ware_itm,MAX_WAREHOUSE_ITEMS);
buy_item_quit;
dec(money,item_cost);
img_update[1]:=true;
img_update[3]:=true;
img_update[4]:=true;
end;
procedure buy_item_cancel;
begin
buy_item_quit;
end;

procedure buy_item_init(itm:PPitem;x0,y0:integer);
var
  name:ansistring;
label lbl1,lbl2;
begin
dialog_x0:=x0;
dialog_y0:=y0;
//g_timer_stop(main_timer);
buying_item:=itm;
item_cost:=round(calc_item_cost(buying_item^)*(2.1-plpilot^.sk.merchant));
enough_money:=money>=item_cost;
if not enough_money then add_sound('sfx\notenoughmoney.wav',false,false,0,0);
cursor.visible:=false;
create_scr_img_onstation(scr_img);
cursor.visible:=true;
main_loop_proc:=@buy_item_loop;
buying_scr_img:=createimageWH(getimagewidth(scr_img),getimageheight(scr_img));
fillimage(buying_scr_img,TRANSPARENCY_COLOR);
//
case itm^^.type_ of
  ITM_WEAPON:name:=bullet_type[itm^^.indx]^.iti.name;
  ITM_FLD_GEN:name:=fld_gen[itm^^.indx]^.iti.name;
  end;
dialog_sx:=gf_textlen(name,font3)+getmaxx div 60;
if dialog_sx<getmaxx div 6 then dialog_sx:=getmaxx div 6;
//buttons
b1:=bt_new(BTT_REGULAR,' ',false,0,0,buying_scr_img);
b1^.act_proc:=@buy_item_buy;
b1^.img:=gf_text_img('ok',font3);
b1^.press_img:=b1^.img;
b1^.w:=dialog_sx div 2;
b1^.h:=getimageheight(b1^.img);
bt_setpos(b1,dialog_x0+dialog_sx div 4-b1^.w div 2,dialog_y0+font3^.h*10 div 2);
//
b2:=bt_new(BTT_REGULAR,' ',false,0,0,buying_scr_img);
b2^.act_proc:=@buy_item_cancel;
b2^.img:=gf_text_img('cancel',font3);
b2^.press_img:=b2^.img;
b2^.w:=dialog_sx div 2;
b2^.h:=getimageheight(b2^.img);
bt_setpos(b2,dialog_x0+dialog_sx*3 div 4-b2^.w div 2,dialog_y0+font3^.h*10 div 2);
//
dialog_sy:=round((3+2.5)*font3^.h)+b1^.h;
dialog_img:=createimageWH(dialog_sx,dialog_sy);
imagefillimage(dialog_img,bg_img3);
gf_outtext(dialog_img,dialog_sx div 2,font3^.h div 2,font3,1,0,'buy',255);
gf_outtext(dialog_img,dialog_sx div 2,font3^.h*4 div 2,font3,1,0,name,255);
gf_outtext(dialog_img,dialog_sx div 2,font3^.h*7 div 2,font3,1,0,'cost: '+to_str(item_cost),255);
if enough_money then goto lbl2;
lbl1:
dialog_sy:=round((3+2)*font3^.h);
dialog_img:=createimageWH(dialog_sx,dialog_sy);
imagefillimage(dialog_img,bg_img3);
gf_outtext(dialog_img,dialog_sx div 2,font3^.h div 2,font3,1,0,name,255);
gf_outtext(dialog_img,dialog_sx div 2,font3^.h*4 div 2,font3,1,0,'cost: '+to_str(item_cost),255);
gf_coladd.r:=80;
with gf_colsub do begin b:=70;g:=60;end;
gf_outtext(dialog_img,dialog_sx div 2,font3^.h*7 div 2,font3,1,0,'not enough money',255);
gf_reset_colors;
lbl2:
draw_frame(dialog_img,smfr,nil,0,0,dialog_sx-1,dialog_sy-1);
draw_image_alpha(scr_img,dialog_img,dialog_x0,dialog_y0,200);
end;

procedure buy_item_quit;
begin
destroyimage(dialog_img);
destroyimage(buying_scr_img);
destroy_button(b1);
destroy_button(b2);
main_loop_proc:=@onstation_loop;
end;

procedure buy_item_loop;
var c:char;
label end_;
begin
update_mouse_input;
if enough_money then
  begin
  update_button(b1,pmb,mb,mx,my);
  if main_loop_proc<>@buy_item_loop then goto end_;
  update_button(b2,pmb,mb,mx,my);
  if main_loop_proc<>@buy_item_loop then goto end_;
  end;
//
if keypressed then
  begin
  c:=readkey;
  if enough_money then
  case c of
    #13,'y','Y':buy_item_buy;
    else buy_item_cancel;
    end
  else buy_item_cancel;
  end;
if (mb=0)and(pmb<>0)and not enough_money then buy_item_cancel else
if mouse_click(1)and(ismouseinarea(dialog_x0,dialog_y0,dialog_x0+dialog_sx,dialog_y0+dialog_sy)=0) then buy_item_cancel;
//
if main_loop_proc<>@buy_item_loop then goto end_;
composeimage(buying_scr_img,scr_img,0,0);
if enough_money then
  begin
  draw_button(b1);
  draw_button(b2);
  end;
with cursor do if visible then
  putimage_wmask(buying_scr_img,img^,mask^,mx,my,255);
putimage(0,0,buying_scr_img);
end_:
end;
