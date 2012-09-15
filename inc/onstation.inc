procedure ppitem_array_sort_by_cost(var ar:array of PPitem;len:integer);forward;
function get_mouse_image_part:integer;
begin
get_mouse_image_part:=mx div (getmaxx div 4+1)+1+4*(my div (getmaxy div 2))
end;

const MAX_BUTTONS=25;
var
  console:boolean;
  console_script:Tscript;
  console_str:ansistring;
  img_part:array[1..8]of pimage;
  img_update:array[1..8]of boolean;
  ship_slot_img:array[0..10]of pimage;
  slot_sx,slot_sy,mouse_slot,ship_window_h,ship_window_w:integer;
  inv_offs,ware_offs,avail_lvl_offs,pass_lvl_offs,mouse_inv_slot,mouse_av_lvl,
    mouse_pa_lvl,selected_lvl,view_enemy_indx,mouse_ware_slot:integer;
  mitem,view_itm:PPitem;
  viewing_item:boolean;
  //buttons
  bt:array[0..MAX_BUTTONS-1]of Pbutton;
  inv_slider,avlvl_slider,palvl_slider,ware_slider,
    attr_inc_but1,attr_inc_but2,attr_inc_but3,attr_inc_but4,
    view_enemy_Lbut,view_enemy_Rbut,menu_bt_quit,menu_bt_save,
    menu_bt_load,menu_bt_newgame,shipattr1but,shipattr2but,shipattr3but,
    shipattr4but,shipattr5but,menu_bt_controls:Pbutton;
  //interface
  small_slot_img,item_view_img,bg_img,bg_img2,bg_img3:pimage;
  sep:Tsep;
  //GATE
  gateGIF:PmediaGIF;
  gateimg:array[1..100]of pimage;
  gatebutton:Pbutton;
  gatetime:double;
  gateframe,gateframes,gateframetime:integer;

procedure update_mouse_pointer;
begin
with cursor do
  if mitem<>nil then
  begin
  case mitem^^.type_ of
    ITM_WEAPON:begin
      img:=@bullet_type[mitem^^.indx]^.iti.img;
      mask:=@bullet_type[mitem^^.indx]^.iti.img_mask;
      end;
    ITM_FLD_GEN:begin
      img:=@fld_gen[mitem^^.indx]^.iti.img;
      mask:=@fld_gen[mitem^^.indx]^.iti.img_mask;
      end;
    end;
  dx:=-getimagewidth(img^)div 2;
  dy:=-getimageheight(img^)div 2;
  end
else
  begin
  dx:=0;dy:=0;
  img:=@dimg;mask:=@dmask;
  end;
end;

procedure onstation_quit(save:boolean);
var i:integer;
begin
for i:=1 to 100 do destroyimage(gateimg[i]);
destroyimage(small_slot_img);
for i:=0 to 10 do destroyimage(ship_slot_img[i]);
destroy_sep(sep);
destroyimage(bg_img);
destroyimage(bg_img2);
destroyimage(bg_img3);
for i:=1 to 8 do destroyimage(img_part[i]);
if plship<>nil then plship^.life:=ship_type[plship^.type_]^.life;
if save then save_game('saved_games\autosave-ascention');
end;

procedure main_quit_attempt;
begin
main_end:=true;
end;

procedure start_selected_level;
begin
if mitem<>nil then
  begin
  destroy_item(mitem^);
  mitem:=nil;
  update_mouse_pointer;
  end
else
if(selected_lvl<>-1)then
  begin
  onstation_quit(true);
  load_level(selected_lvl);
  prepare_level;
  end;
end;

function calc_shipattr_cost(attr:longint):longint;
begin
calc_shipattr_cost:=round((attr/10)*power(2,attr/70));
end;
procedure shipattr1_inc;
var cost:longint;
begin
cost:=calc_shipattr_cost(ship_type[plship^.type_]^.life);
if cost<=money then
	begin
  dec(money,cost);
  inc(ship_type[plship^.type_]^.life,1);
  img_update[1]:=true;
  end;
end;
procedure shipattr2_inc;
var cost:longint;
begin
cost:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.move_speed*100));
if cost<=money then
if SKMAX_MOVESPEED>ship_type[plship^.type_]^.sk.move_speed then
	begin
  dec(money,cost);
  incf(ship_type[plship^.type_]^.sk.move_speed,0.01);
  img_update[1]:=true;
  end else shipattr1but^.state:=BTS_DISABLED;
end;
procedure shipattr3_inc;
var cost:longint;
begin
cost:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.bullet_dmg*100));
if cost<=money then
	begin
  dec(money,cost);
  incf(ship_type[plship^.type_]^.sk.bullet_dmg,0.01);
  img_update[1]:=true;
  end;
end;
procedure shipattr4_inc;
var cost:longint;
begin
cost:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.bullet_speed*100));
if cost<=money then
	begin
  dec(money,cost);
  incf(ship_type[plship^.type_]^.sk.bullet_speed,0.01);
  img_update[1]:=true;
  end;
end;
procedure shipattr5_inc;
var cost:longint;
begin
cost:=calc_shipattr_cost(round(ship_type[plship^.type_]^.sk.attack_speed*100));
if cost<=money then
	begin
  dec(money,cost);
  incf(ship_type[plship^.type_]^.sk.attack_speed,0.01);
  img_update[1]:=true;
  end;
end;

procedure attr_inc1;
var d:double;
begin
if skill_points>0 then
with plpilot^.sk do
if SKMAX_MOVESPEED>move_speed then
  begin
  dec(skill_points);
  if move_speed<1.5 then d:=3/100 else d:=2/100;
  move_speed:=move_speed+d;
  end else attr_inc_but1^.state:=BTS_DISABLED;
end;
procedure attr_inc2;
var d:double;
begin
if skill_points>0 then
with plpilot^.sk do
  begin
  dec(skill_points);
  if bullet_dmg<1.5 then d:=2.5/100 else d:=1.5/100;
  bullet_dmg:=bullet_dmg+d;
  end;
end;
procedure attr_inc3;
var d:double;
begin
if skill_points>0 then
with plpilot^.sk do
  begin
  dec(skill_points);
  if bullet_speed<1.5 then d:=2.5/100 else d:=1.5/100;
  bullet_speed:=bullet_speed+d;
  end;
end;
procedure attr_inc4;
begin
if skill_points>0 then
  begin
  dec(skill_points);
  plpilot^.sk.merchant:=plpilot^.sk.merchant+2/100;
  img_update[3]:=true;
  img_update[4]:=true;
  end;
end;
procedure scroll_enemy_indxL;
var i:integer;
begin
if selected_lvl<>-1 then
  begin
  i:=view_enemy_indx;
  repeat dec(i) until (i<0)or(lvl_enemy_types[i].type_<>-1);
  if (i>=0)and(lvl_enemy_types[i].type_<>-1)then view_enemy_indx:=i;
  end;
end;
procedure scroll_enemy_indxR;
var i:integer;
begin
if selected_lvl<>-1 then
  begin
  i:=view_enemy_indx;
  repeat inc(i) until (i>=MAX_LEVEL_ENEMY_TYPES)or(lvl_enemy_types[i].type_<>-1);
  if (i<MAX_LEVEL_ENEMY_TYPES)and(lvl_enemy_types[i].type_<>-1)then view_enemy_indx:=i;
  end;
end;

procedure warehouse_clear(perc:integer);
var i:integer;
begin
perc:=perc*100;
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do
  if (ware_itm[i]<>nil)and(random(10000 div perc)div 100=0) then
    begin
    destroy_item(ware_itm[i]^);
    ware_itm[i]:=nil;
    end;
end;

procedure warehouse_createitems(kol:integer);
var i:integer;
  itm:PPitem;
begin
if kol>MAX_WAREHOUSE_ITEMS then kol:=MAX_WAREHOUSE_ITEMS;
for i:=1 to kol do
  begin
  case random(2)of
    0:itm:=new_item(ITM_WEAPON,-1);
    1:itm:=new_item(ITM_FLD_GEN,-1);
    end;
  warehouse_add_item(itm);
  case itm^^.type_ of
    ITM_WEAPON:modify_weapon(itm,random(exp)div 2);
    end;
  end;
ppitem_array_sort_by_cost(ware_itm,MAX_WAREHOUSE_ITEMS);
end;

procedure show_controls;
begin
controls_init(main_loop_proc);
main_loop_proc:=@controls_loop;
end;

procedure onstation_init(update_warehouse,save:boolean);
var i:integer;
	t:longint;
begin
remove_sound(main_sound);
main_sound:=add_sound('sfx\sw.mp3',true,false,0,0);
for i:=0 to MAX_SHIP_SLOTS-1 do plship^.slot_cooldown[i]:=0;
plship^.life:=ship_type[plship^.type_]^.life;
plship^.fld:=0;
if plship^.fld_eff<>nil then
	begin
	destroy_effect(plship^.fld_eff^);
  plship^.fld_eff:=nil;
  end;
//
console_str:='';
console:=false;
//
if update_warehouse then
  begin
  warehouse_clear(33);
  warehouse_createitems(10);
  end;
//gate
gateframetime:=50;
new(gateGIF,openmedia('gfx\other\gate.gif'));
while not gategif^.endofmedia do gateimg[gategif^.getcurframe+1]:=cloneimage(gategif^.grabframe);
gateframes:=gategif^.getnumframe;
gateframe:=1;
gatetime:=0;
dispose(gateGIF,closemedia);
//
for i:=1 to 8 do img_part[i]:=createimageWH(getmaxx div 4,getmaxy div 2);
for i:=1 to 8 do img_update[i]:=true;
loadimagefile(itdetect,'gfx\other\item_slot.gif',small_slot_img,0);
loadimagefile(itdetect,'gfx\other\ship_slot.gif',ship_slot_img[ITM_WEAPON],1);
loadimagefile(itdetect,'gfx\other\ship_slot.gif',ship_slot_img[ITM_SCRIPT],2);
loadimagefile(itdetect,'gfx\other\ship_slot.gif',ship_slot_img[ITM_FLD_GEN],3);
slot_sx:=getimagewidth(small_slot_img);
slot_sy:=getimageheight(small_slot_img);
cursor.visible:=true;
inv_offs:=0;
ware_offs:=0;
avail_lvl_offs:=0;
pass_lvl_offs:=0;
mitem:=nil;
loadimagefile(itdetect,'gfx\other\bg1.gif',bg_img,0);
loadimagefile(itdetect,'gfx\other\bg2.gif',bg_img2,0);
loadimagefile(itdetect,'gfx\other\bg3.gif',bg_img3,0);
framemask:=nil;
load_sep(sep,'gfx\other\sep1.gif');
//delete free
for t:=1 to MAX_LEVELS do
for i:=MAX_LEVELS-1 downto 1 do
	begin
  if (avail_lvl[i]<>-1)and(avail_lvl[i-1]=-1) then
  	begin
    avail_lvl[i-1]:=avail_lvl[i];
    avail_lvl[i]:=-1;
    end;
  if (pass_lvl[i]<>-1)and(pass_lvl[i-1]=-1) then
  	begin
    pass_lvl[i-1]:=pass_lvl[i];
    pass_lvl[i]:=-1;
    end;
  end;
//sorting level lists
for i:=0 to MAX_LEVELS-2 do
for t:=i to MAX_LEVELS-2 do
	begin
	if (avail_lvl[t]>avail_lvl[t+1])and(avail_lvl[t+1]<>-1) then swap_int(avail_lvl[t],avail_lvl[t+1]);
  if (pass_lvl[t]>pass_lvl[t+1])and(pass_lvl[t+1]<>-1) then swap_int(pass_lvl[t],pass_lvl[t+1]);
  end;
//
selected_lvl:=avail_lvl[0];
if selected_lvl=-1 then selected_lvl:=pass_lvl[0];
if selected_lvl<>-1 then load_level(selected_lvl);
view_enemy_indx:=0;
//BUTTONS
for i:=0 to MAX_BUTTONS-1 do bt[i]:=nil;
i:=0;
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\slider.gif',false,0,2,scr_img);
bt_setpos(bt[i],getmaxx*3 div 4-bt[i]^.w-mainframe^.w2+2,mainframe^.w1+slot_sy+1);
bt[i]^.y_min:=bt[i]^.y;
bt[i]^.y_max:=getmaxy div 2-mainframe^.w3;
inv_slider:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\slider.gif',false,0,2,scr_img);
bt_setpos(bt[i],getmaxx-bt[i]^.w-mainframe^.w2+2,mainframe^.w1+slot_sy+1);
bt[i]^.y_min:=bt[i]^.y;
bt[i]^.y_max:=getmaxy div 2-mainframe^.w3;
ware_slider:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\slider.gif',false,0,2,scr_img);
bt_setpos(bt[i],getmaxx div 4-bt[i]^.w-mainframe^.w2+2,getmaxy div 2+mainframe^.w1+slot_sy+1);
bt[i]^.y_min:=bt[i]^.y;
bt[i]^.y_max:=getmaxy-mainframe^.w3;
avlvl_slider:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\slider.gif',false,0,2,scr_img);
bt_setpos(bt[i],getmaxx div 2-bt[i]^.w-mainframe^.w2+2,getmaxy div 2+mainframe^.w1+slot_sy+1);
bt[i]^.y_min:=bt[i]^.y;
bt[i]^.y_max:=getmaxy-mainframe^.w3;
palvl_slider:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
gatebutton:=bt[i];
gatebutton^.act_proc:=@start_selected_level;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\plus.gif',false,0,0,scr_img);
attr_inc_but1:=bt[i];
attr_inc_but1^.act_proc:=@attr_inc1;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\plus.gif',false,0,0,scr_img);
attr_inc_but2:=bt[i];
attr_inc_but2^.act_proc:=@attr_inc2;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\plus.gif',false,0,0,scr_img);
attr_inc_but3:=bt[i];
attr_inc_but3^.act_proc:=@attr_inc3;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\plus.gif',false,0,0,scr_img);
attr_inc_but4:=bt[i];
attr_inc_but4^.act_proc:=@attr_inc4;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\left_arrow.gif',true,1,0,scr_img);
view_enemy_Lbut:=bt[i];
view_enemy_Lbut^.act_proc:=@scroll_enemy_indxL;
inc(i);
bt[i]:=bt_new(BTT_REGULAR,'gfx\other\right_arrow.gif',true,1,0,scr_img);
view_enemy_Rbut:=bt[i];
view_enemy_Rbut^.act_proc:=@scroll_enemy_indxR;
//ship attributes buttons
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@shipattr1_inc;
shipattr1but:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@shipattr2_inc;
shipattr2but:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@shipattr3_inc;
shipattr3but:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@shipattr4_inc;
shipattr4but:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@shipattr5_inc;
shipattr5but:=bt[i];
//menu buttons
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@new_game;
bt[i]^.img:=gf_text_img('new game',font2);
bt[i]^.press_img:=bt[i]^.img;
bt[i]^.w:=getmaxx*3 div 16;
bt[i]^.h:=getimageheight(bt[i]^.img);
menu_bt_newgame:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@main_quit_attempt;
bt[i]^.img:=gf_text_img('quit game',font2);
bt[i]^.press_img:=bt[i]^.img;
bt[i]^.w:=getmaxx*3 div 16;
bt[i]^.h:=getimageheight(bt[i]^.img);
menu_bt_quit:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@saving_game;
bt[i]^.img:=gf_text_img('save game',font2);
bt[i]^.press_img:=bt[i]^.img;
bt[i]^.w:=getmaxx*3 div 16;
bt[i]^.h:=getimageheight(bt[i]^.img);
menu_bt_save:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@loading_game;
bt[i]^.img:=gf_text_img('load game',font2);
bt[i]^.press_img:=bt[i]^.img;
bt[i]^.w:=getmaxx*3 div 16;
bt[i]^.h:=getimageheight(bt[i]^.img);
menu_bt_load:=bt[i];
inc(i);
bt[i]:=bt_new(BTT_REGULAR,' ',false,0,0,scr_img);
bt[i]^.act_proc:=@show_controls;
bt[i]^.img:=gf_text_img('controls',font2);
bt[i]^.press_img:=bt[i]^.img;
bt[i]^.w:=getmaxx*3 div 16;
bt[i]^.h:=getimageheight(bt[i]^.img);
menu_bt_controls:=bt[i];
//
if save then save_game('saved_games\autosave-alighting');
end;

procedure draw_console(dimg:pimage);
var timg:pimage;
  d:integer;
begin
d:=3;
timg:=createimageWH(getmaxx+1,getmaxy div 10);
fillimage(timg,rgbcolorrgb(1,1,1));
draw_image_alpha(dimg,timg,0,0,80);
gf_outtext(dimg,d,getmaxy div 10-d,font3,0,2,console_str,220);
destroyimage(timg);
end;

const MAX_ITEM_CHARACTERISTICS=15;
function create_item_info_image(itm:Pitem):pimage;
var dimg,itm_img,itm_img_mask:pimage;
  col,col2,d,w0,h0,imgd,y,kol_str,max_len,i,j,ds:longint;
  d1,d2:integer;
  tstr,itm_name:ansistring;
  text_list:array[0..MAX_ITEM_CHARACTERISTICS-1]of ansistring;
  color_list:array[0..MAX_ITEM_CHARACTERISTICS-1]of longint;
label end_;
begin
if itm=nil then goto end_;
d:=10;
ds:=font3^.dl*8;
col:=rgbcolorrgb(0,0,255);
col2:=rgbcolorrgb(180,100,20);
itm_name:='unknown item';
i:=0;
case itm^.type_ of
  ITM_WEAPON:with bullet_type[itm^.indx]^ do
    begin
    itm_name:=bullet_type[itm^.indx]^.iti.name;
    max_len:=gf_textlen(itm_name,font2);
    loadimagefile(itdetect,iti.view_img_path,itm_img,0);
    loadimagefile(itdetect,iti.view_img_path,itm_img_mask,2);
    kol_str:=5;
    d1:=mweap_dmg1(bullet_type[itm^.indx],@itm);
    d2:=mweap_dmg2(bullet_type[itm^.indx],@itm);
    if d1>d2 then swap_int(d1,d2);
    text_list[i]:='Damage: '+to_str(d1)+'-'+to_str(d2)+' ('+
      to_str(round(d1*plpilot^.sk.bullet_dmg))+'-'+to_str(round(d2*plpilot^.sk.bullet_dmg))+')';inc(i);
    text_list[i]:='Attack Cooldown: '+to_str(cooldown)+' ms';inc(i);
    if lifetime<>-1 then tstr:=to_str(lifetime)+' ms' else tstr:='Unlimited';
    text_list[i]:='Missile lifetime: '+tstr;inc(i);
    text_list[i]:='Missile speed: '+to_str(vel)+' ('+to_str(round(vel*plpilot^.sk.bullet_speed))+') m/s';inc(i);
    text_list[i]:='Max targets: '+to_str(cross);inc(i);
    for j:=0 to kol_str-1 do color_list[j]:=col;
    if guided then
      begin
      inc(kol_str,3);
      for j:=j+1 to kol_str-1 do color_list[j]:=col2;
      text_list[i]:='Guided missiles:';inc(i);
      text_list[i]:='Guide time: '+to_str(guide_time)+' ms';inc(i);
      text_list[i]:='Guide distance: '+to_str(guide_dist)+' m';inc(i);
      end;
    end;//weapon
  ITM_FLD_GEN:with fld_gen[itm^.indx]^ do
    begin
    itm_name:=fld_gen[itm^.indx]^.iti.name;
    max_len:=gf_textlen(itm_name,font2);
    loadimagefile(itdetect,iti.view_img_path,itm_img,0);
    loadimagefile(itdetect,iti.view_img_path,itm_img_mask,2);
    kol_str:=3;
    text_list[i]:='Damage absorbing: '+to_str(round(100-koef*100))+'%';inc(i);
    text_list[i]:='Reflect chance: '+to_str(refl_chance)+'%';inc(i);
    text_list[i]:='Using cooldown: '+to_str(cooldown div 1000)+' s';inc(i);
    end;//fld gen
  end;//case
h0:=(kol_str)*(font3^.h+ds)+font2^.h+3*d;
for i:=0 to kol_str-1 do
   if max_len<gf_textlen(text_list[i],font3)then max_len:=gf_textlen(text_list[i],font3);
if h0<2*d+getimageheight(itm_img) then h0:=2*d+getimageheight(itm_img);
w0:=getimagewidth(itm_img)+5*d+round(max_len);
create_item_info_image:=createimageWH(w0,h0);
dimg:=create_item_info_image;
setimagetransparencycolor(dimg,TRANSPARENCY_COLOR);
imagefillimage(dimg,bg_img);
putimage_wmask(dimg,itm_img,itm_img_mask,d,d,255);
for i:=0 to getimagewidth(dimg)-1 do
for j:=0 to getimageheight(dimg)-1 do
  if imagegetpixel(dimg,i,j)=0 then imageputpixel(dimg,i,j,rgbcolorrgb(5,5,5));
imgd:=getimagewidth(itm_img)+2*d;
gf_outtext(dimg,imgd+(w0-imgd-d)div 2,d,font2,1,0,itm_name,220);
y:=font2^.h+2*d;
for i:=0 to kol_str-1 do
  begin
  if i>4 then
    begin
    with gf_coladd do begin b:=60;end;
    with gf_colsub do begin r:=20;g:=20;b:=20;end;
    end;
  gf_outtext(dimg,imgd,y,font3,0,0,text_list[i],180);
  inc(y,font3^.h+ds);
  gf_reset_colors;
  end;
destroyimage(itm_img);
destroyimage(itm_img_mask);
draw_frame(dimg,mainframe,framemask,0,0,w0-1,h0-1);
end_:
end;

function create_menu_image(x0,y0,w0,h0:integer):pimage;
var dimg:pimage;
  d:integer;
begin
create_menu_image:=createimageWH(w0,h0);
dimg:=create_menu_image;
imagefillimage(dimg,bg_img2);
d:=h0 div 15;
bt_setpos(menu_bt_newgame,x0+w0 div 2-menu_bt_newgame^.w div 2,y0+h0 div 5);
bt_setpos(menu_bt_save,x0+w0 div 2-menu_bt_save^.w div 2,menu_bt_newgame^.y+menu_bt_newgame^.h+d);
bt_setpos(menu_bt_load,x0+w0 div 2-menu_bt_load^.w div 2,menu_bt_save^.y+menu_bt_save^.h+d);
bt_setpos(menu_bt_controls,x0+w0 div 2-menu_bt_controls^.w div 2,menu_bt_load^.y+menu_bt_load^.h+d);
bt_setpos(menu_bt_quit,x0+w0 div 2-menu_bt_quit^.w div 2,menu_bt_controls^.y+menu_bt_controls^.h+d);
end;

function create_player_info_image(x0,y0,w,h:integer):pimage;
var dimg:pimage;
  p1h,p2h,p3h,p4h,p1y,p2y,p3y,p4y,col,f,d:longint;
begin
f:=w div 20;
col:=rgbcolorrgb(0,0,255);
p1h:=round(h/8);
p2h:=round(h/2)-p1h;
p3h:=round(h/10);
p4h:=round(h/2)-p3h;
p1y:=0;
p2y:=p1h;
p3y:=p1h+p2h;
p4y:=p1h+p2h+p3h;
create_player_info_image:=createimageWH(w,h);
dimg:=create_player_info_image;
imagefillimage(dimg,bg_img2);
draw_Hsep(dimg,sep,w div 40,p2y,w*38 div 40);
draw_Hsep(dimg,sep,w div 40,p3y,w*38 div 40);
draw_Hsep(dimg,sep,w div 40,p4y,w*38 div 40);

d:=(p2h-5*font3^.h)div 6;
gf_outtext(dimg,f,p2y+d,font3,0,0,'LEVEL '+to_str(pllvl),180);
gf_outtext(dimg,f,p2y+2*(d+font3^.h)-font3^.h,font3,0,0,'EXP '+to_str(exp)+'/'+to_str(next_exp),180);
gf_outtext(dimg,f,p2y+3*(d+font3^.h)-font3^.h,font3,0,0,'SKILL POINTS '+to_str(skill_points),180);
gf_outtext(dimg,f,p2y+4*(d+font3^.h)-font3^.h,font3,0,0,'CREDITS '+to_str(money),180);
gf_outtext(dimg,f,p2y+5*(d+font3^.h)-font3^.h,font3,0,0,'Repair kits '+to_str(repair_kol),180);
with plpilot^ do
  begin
  gf_outtext(dimg,w div 2,p1h div 2,font3,1,1,plpilot^.name,220);
  gf_outtext(dimg,w div 2,p3y+p3h div 2,font3,1,1,'SKILLS',220);
  d:=(p4h-4*font3^.h)div 5;
  gf_outtext(dimg,f,p4y+d,font3,0,0,'SHIP CONTROL ',180);
  gf_outtext(dimg,w*3 div 4,p4y+d,font3,2,0,to_str(round(sk.move_speed*100)),180);
  gf_outtext(dimg,f,p4y+2*(d+font3^.h)-font3^.h,font3,0,0,'ATTACK SKILL ',180);
  gf_outtext(dimg,w*3 div 4,p4y+2*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.bullet_dmg*100)),180);
  gf_outtext(dimg,f,p4y+3*(d+font3^.h)-font3^.h,font3,0,0,'MISSILE SPEED ',180);
  gf_outtext(dimg,w*3 div 4,p4y+3*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.bullet_speed*100)),180);
  gf_outtext(dimg,f,p4y+4*(d+font3^.h)-font3^.h,font3,0,0,'MERCHANT ',180);
  gf_outtext(dimg,w*3 div 4,p4y+4*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.merchant*100)),180);
  //buttons
  bt_setpos(attr_inc_but1,x0+w*3 div 4+f div 2,y0+p4y+d+font3^.h div 2-attr_inc_but1^.h div 2);
  bt_setpos(attr_inc_but2,x0+w*3 div 4+f div 2,y0+p4y+2*(d+font3^.h)-font3^.h div 2-attr_inc_but2^.h div 2);
  bt_setpos(attr_inc_but3,x0+w*3 div 4+f div 2,y0+p4y+3*(d+font3^.h)-font3^.h div 2-attr_inc_but3^.h div 2);
  bt_setpos(attr_inc_but4,x0+w*3 div 4+f div 2,y0+p4y+4*(d+font3^.h)-font3^.h div 2-attr_inc_but4^.h div 2);
  end;
end;

function get_slot_x(i,w:integer):integer;
begin
case i of
  0..3:get_slot_x:=w div 6-slot_sx div 2;
  6..9:get_slot_x:=w*5 div 6-slot_sx div 2;
  4..5:get_slot_x:=w div 2-slot_sx div 2;
  end;
end;
function get_slot_y(i,h:integer):integer;
begin
case i of
  0..3:get_slot_y:=(i+1)*(h-slot_sy+5)div 4-slot_sy div 2;
  6..9:get_slot_y:=(i-5)*(h-slot_sy+5)div 4-slot_sy div 2;
  4:get_slot_y:=(h-slot_sy+5)div 4-slot_sy div 2;
  5:get_slot_y:=4*(h-slot_sy+5)div 4-slot_sy div 2;
  end;
end;

function create_ship_info_image(x0,y0,w0,h0:integer):pimage;
var p1h,p2h,p3h,p1y,p2y,p3y,col,col2,f,a,d:longint;
  dimg:pimage;
begin
f:=w0 div 20;
col:=rgbcolorrgb(0,0,255);
create_ship_info_image:=createimageWH(w0,h0);
dimg:=create_ship_info_image;
imagefillimage(dimg,bg_img2);
p1h:=round(h0/2);
p2h:=round(h0/10);
p3h:=round(h0/2)-p2h;
p1y:=0;
p2y:=p1h;
p3y:=p1h+p2h;
draw_Hsep(dimg,sep,w0 div 40,p2y,w0*38 div 40);
draw_Hsep(dimg,sep,w0 div 40,p3y,w0*38 div 40);
ship_window_h:=p1h;
ship_window_w:=w0;

d:=(p3h-5*font3^.h)div 6;
with plship^ do
  begin
  putimage_wmask(dimg,ship_type[plship^.type_]^.g[0].img,ship_type[plship^.type_]^.g[0].img_mask,w0 div 2-ship_w(plship)div 2,p1h div 2-ship_h(plship)div 2,255);
  for i:=0 to MAX_SHIP_SLOTS-1 do if slot[i].exist then
  if(slot[i].itm_type=ITM_WEAPON)then
    begin
    col2:=rgbcolorrgb(128,0,0);
    imagebar(dimg,w0 div 2+plship^.slot[i].dx-1,p1h div 2+slot[i].dy-1,w0 div 2+slot[i].dx+1,p1h div 2+slot[i].dy+1,col2);
    end;
  for i:=0 to MAX_SHIP_SLOTS-1 do if slot[i].exist then
  if (mouse_slot=i)and(slot[i].itm_type=ITM_WEAPON) then
    begin
    col2:=rgbcolorrgb(255,0,0);
    imagebar(dimg,w0 div 2+slot[i].dx-1,p1h div 2+slot[i].dy-1,w0 div 2+slot[i].dx+1,p1h div 2+slot[i].dy+1,col2);
    end;
  for i:=0 to 9 do
  if slot[i].exist then
    begin
    if (mitem<>nil)and(mitem^^.type_=slot[i].itm_type)then a:=200 else a:=120;
    draw_image_alpha(dimg,ship_slot_img[slot[i].itm_type],get_slot_x(i,w0),get_slot_y(i,p1h),a);
    if (slot[i].itm<>nil)and(slot[i].itm^^.indx<>-1)then
    case slot[i].itm^^.type_ of
      ITM_WEAPON:with bullet_type[slot[i].itm^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,get_slot_x(i,w0)+slot_sx div 2-getimagewidth(iti.img)div 2,get_slot_y(i,p1h)+slot_sy div 2-getimageheight(iti.img)div 2,255);
      ITM_FLD_GEN:with fld_gen[slot[i].itm^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,get_slot_x(i,w0)+slot_sx div 2-getimagewidth(iti.img)div 2,get_slot_y(i,p1h)+slot_sy div 2-getimageheight(iti.img)div 2,255);
      end;
    if (slot[i].hotkey<>#0)and(slot[i].itm<>nil) then
      begin
      imagerectangle(dimg,get_slot_x(i,w0),get_slot_y(i,p1h),get_slot_x(i,w0)+slot_sx-1,get_slot_y(i,p1h)+slot_sy-1,rgbcolorrgb(100,100,30));
      gf_outtext(dimg,get_slot_x(i,w0)+2,get_slot_y(i,p1h)+2,font3,0,0,slot[i].hotkey,128);
      end;
    end;
  with ship_type[plship^.type_]^ do
    begin
    gf_outtext(dimg,w0 div 2,p2y+p2h div 2,font3,1,1,'SHIP CHARACTERISTICS',220);
    gf_outtext(dimg,f,p3y+d,font3,0,0,'Solidity',180);
    gf_outtext(dimg,w0*3 div 4,p3y+d,font3,2,0,to_str(life),180);
    gf_outtext(dimg,f,p3y+2*(d+font3^.h)-font3^.h,font3,0,0,'Contollability',180);
    gf_outtext(dimg,w0*3 div 4,p3y+2*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.move_speed*100)),180);
    gf_outtext(dimg,f,p3y+3*(d+font3^.h)-font3^.h,font3,0,0,'Attack Damage',180);
    gf_outtext(dimg,w0*3 div 4,p3y+3*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.bullet_dmg*100)),180);
    gf_outtext(dimg,f,p3y+4*(d+font3^.h)-font3^.h,font3,0,0,'Missile Speed',180);
    gf_outtext(dimg,w0*3 div 4,p3y+4*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.bullet_speed*100)),180);
    gf_outtext(dimg,f,p3y+5*(d+font3^.h)-font3^.h,font3,0,0,'Rate Of fire',180);
    gf_outtext(dimg,w0*3 div 4,p3y+5*(d+font3^.h)-font3^.h,font3,2,0,to_str(round(sk.attack_speed*100)),180);
    end;
  //buttons
  {$include shipattr_buttons.inc}
  bt_setpos(shipattr1but,x0+w0*3 div 4+f,y0+p3y+d+font3^.h div 2-shipattr1but^.h div 2);
  bt_setpos(shipattr2but,x0+w0*3 div 4+f,y0+p3y+2*(d+font3^.h)-font3^.h+font3^.h div 2-shipattr2but^.h div 2);
  bt_setpos(shipattr3but,x0+w0*3 div 4+f,y0+p3y+3*(d+font3^.h)-font3^.h+font3^.h div 2-shipattr3but^.h div 2);
  bt_setpos(shipattr4but,x0+w0*3 div 4+f,y0+p3y+4*(d+font3^.h)-font3^.h+font3^.h div 2-shipattr4but^.h div 2);
  bt_setpos(shipattr5but,x0+w0*3 div 4+f,y0+p3y+5*(d+font3^.h)-font3^.h+font3^.h div 2-shipattr5but^.h div 2);
  end;
end;

function create_inventory_image(w0,h0:integer):pimage;
var i,d:integer;
  col:longint;
  dimg:pimage;
begin
col:=rgbcolorrgb(0,0,255);
create_inventory_image:=createimageWH(w0,h0);
dimg:=create_inventory_image;
imagefillimage(dimg,bg_img3);
gf_outtext(dimg,w0 div 2,slot_sy div 2,font3,1,1,'Storehouse',220);
draw_frame(dimg,smfr,nil,0,0,w0-1,slot_sy);
for i:=0 to MAX_INVENTORY_ITEMS-1 do
  begin
  inc(i);
  draw_image_alpha(dimg,small_slot_img,0,i*slot_sy,130);
  draw_frame(dimg,smfr,nil,0,i*slot_sy,slot_sx-1,(i+1)*slot_sy);
  draw_frame(dimg,smfr,nil,slot_sx,i*slot_sy,w0-1-inv_slider^.w,(i+1)*slot_sy);
  if (i-1+inv_offs<MAX_INVENTORY_ITEMS)and(inv[i-1+inv_offs]<>nil) then
  case inv[i-1+inv_offs]^^.type_ of
    ITM_WEAPON:begin
      with bullet_type[inv[i-1+inv_offs]^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,slot_sx div 2-getimagewidth(iti.img)div 2,i*slot_sy+slot_sy div 2-getimageheight(iti.img)div 2,255);
      d:=4;
      gf_outtext(dimg,slot_sx+d,i*slot_sy+d,font4,0,0,bullet_type[inv[i-1+inv_offs]^^.indx]^.iti.name,180);
      gf_outtext(dimg,w0-inv_slider^.w-d,(i+1)*slot_sy-d,font4,2,2,to_str(calc_item_cost(inv[i-1+inv_offs]^)),180);
      end;
    ITM_FLD_GEN:begin
      with fld_gen[inv[i-1+inv_offs]^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,slot_sx div 2-getimagewidth(iti.img)div 2,i*slot_sy+slot_sy div 2-getimageheight(iti.img)div 2,255);
      d:=4;
      gf_outtext(dimg,slot_sx+d,i*slot_sy+d,font4,0,0,fld_gen[inv[i-1+inv_offs]^^.indx]^.iti.name,180);
      gf_outtext(dimg,w0-inv_slider^.w-d,(i+1)*slot_sy-d,font4,2,2,to_str(calc_item_cost(inv[i-1+inv_offs]^)),180);
      end;
    end;//case
  dec(i);
  end;
draw_frame(dimg,smfr,nil,w0-inv_slider^.w,slot_sy+1,w0-1,h0);
end;

function create_level_info_image(x0,y0,w0,h0:integer):pimage;
var
  p1h,p2h,p3h,p4h,p5h,p1y,p2y,p3y,p4y,p5y,f,d:longint;
  dimg:pimage;
begin
f:=w0 div 20;
p1h:=h0 div 8;
p2h:=h0 div 8;
p3h:=h0 div 8;
p4h:=w0*2 div 5;
p5h:=h0-p1h-p2h-p3h-p4h;
p2y:=p1h;
p3y:=p1h+p2h;
p4y:=p1h+p2h+p3h;
p5y:=p1h+p2h+p3h+p4h;
create_level_info_image:=createimageWH(w0,h0);
dimg:=create_level_info_image;
imagefillimage(dimg,bg_img2);
if selected_lvl<>-1 then
  begin
  gf_outtext(dimg,w0 div 2,p1h div 2,font3,1,1,lvl_names[selected_lvl],220);
  draw_Hsep(dimg,sep,w0 div 40,p2y,w0*38 div 40);
  gf_outtext(dimg,f,p2y+p2h div 2,font3,0,1,'Size: '+to_str(lvl_screens)+' pc',220);
  gf_outtext(dimg,f,p3y,font3,0,0,'Enemies: '+to_str(lvl_enemies),220);
//  draw_img_bar(dimg,bg_img3,w0 div 2-p4h div 2,p4y,w0 div 2+p4h div 2,p4y+p4h);
//  draw_frame(dimg,smfr,nil,w0 div 2-p4h div 2,p4y,w0 div 2+p4h div 2,p4y+p4h);
  if view_enemy_indx<>-1 then
  with ship_type[lvl_enemy_types[view_enemy_indx].type_]^ do
    putimage_wmask(dimg,g[0].img,g[0].img_mask,w0 div 2-getimagewidth(g[0].img)div 2,p4y+p4h div 2-getimageheight(g[0].img)div 2,255);
  composeimage(dimg,gateimg[gateframe],w0 div 2-getimagewidth(gateimg[gateframe])div 2,p5y+p5h div 2-getimageheight(gateimg[gateframe])div 2);
  draw_frame(dimg,smfr,nil,w0 div 2-getimagewidth(gateimg[gateframe])div 2,p5y+p5h div 2-getimageheight(gateimg[gateframe])div 2,
    w0 div 2+getimagewidth(gateimg[gateframe])div 2,p5y+p5h div 2+getimageheight(gateimg[gateframe])div 2);
  bt_setpos(gatebutton,x0+w0 div 2-getimagewidth(gateimg[gateframe])div 2,y0+p5y+p5h div 2-getimageheight(gateimg[gateframe])div 2);
  gatebutton^.w:=getimagewidth(gateimg[gateframe]);
  gatebutton^.h:=getimageheight(gateimg[gateframe]);
  bt_setpos(view_enemy_Lbut,x0+w0 div 2-p4h div 2-f-view_enemy_Lbut^.w,y0+p4y+p4h div 2-view_enemy_Lbut^.h div 2);
  bt_setpos(view_enemy_Rbut,x0+w0 div 2+p4h div 2+f,y0+p4y+p4h div 2-view_enemy_Lbut^.h div 2);
  end;
end;

function create_warehouse_image(w0,h0:integer):pimage;
var i,d:integer;
  cost:longint;
  dimg:pimage;
begin
create_warehouse_image:=createimageWH(w0,h0);
dimg:=create_warehouse_image;
imagefillimage(dimg,bg_img3);
gf_outtext(dimg,w0 div 2,slot_sy div 2,font3,1,1,'Warehouse',220);
draw_frame(dimg,smfr,nil,0,0,w0-1,slot_sy);
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do
  begin
  inc(i);
  draw_image_alpha(dimg,small_slot_img,0,i*slot_sy,130);
  draw_frame(dimg,smfr,nil,0,i*slot_sy,slot_sx-1,(i+1)*slot_sy);
  draw_frame(dimg,smfr,nil,slot_sx,i*slot_sy,w0-1-ware_slider^.w,(i+1)*slot_sy);
  if (i-1+ware_offs<MAX_WAREHOUSE_ITEMS)and(ware_itm[i-1+ware_offs]<>nil) then
  case ware_itm[i-1+ware_offs]^^.type_ of
    ITM_WEAPON:begin
      with bullet_type[ware_itm[i-1+ware_offs]^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,slot_sx div 2-getimagewidth(iti.img)div 2,i*slot_sy+slot_sy div 2-getimageheight(iti.img)div 2,255);
      d:=4;
      gf_outtext(dimg,slot_sx+d,i*slot_sy+d,font4,0,0,bullet_type[ware_itm[i-1+ware_offs]^^.indx]^.iti.name,180);
      cost:=round(calc_item_cost(ware_itm[i-1+ware_offs]^)*(2.1-plpilot^.sk.merchant));
      if cost>money then
        begin
        gf_coladd.r:=120;
        with gf_colsub do begin r:=40;g:=30;b:=50;end;
        end;
      gf_outtext(dimg,w0-ware_slider^.w-d,(i+1)*slot_sy-d,font4,2,2,to_str(cost),180);
      gf_reset_colors;
      end;
    ITM_FLD_GEN:begin
      with fld_gen[ware_itm[i-1+ware_offs]^^.indx]^ do
        putimage_wmask(dimg,iti.img,iti.img_mask,slot_sx div 2-getimagewidth(iti.img)div 2,i*slot_sy+slot_sy div 2-getimageheight(iti.img)div 2,255);
      d:=4;
      gf_outtext(dimg,slot_sx+d,i*slot_sy+d,font4,0,0,fld_gen[ware_itm[i-1+ware_offs]^^.indx]^.iti.name,180);
      cost:=round(calc_item_cost(ware_itm[i-1+ware_offs]^)*(2.1-plpilot^.sk.merchant));
      if cost>money then
        begin
        gf_coladd.r:=120;
        with gf_colsub do begin r:=40;g:=30;b:=50;end;
        end;
      gf_outtext(dimg,w0-ware_slider^.w-d,(i+1)*slot_sy-d,font4,2,2,to_str(cost),180);
      gf_reset_colors;
      end;
    end;//case
  dec(i);
  end;
draw_frame(dimg,smfr,nil,w0-ware_slider^.w,slot_sy+1,w0-1,h0);
end;

function create_avail_lvl_image(w0,h0:integer):pimage;
var i:integer;
  col:longint;
  dimg:pimage;
begin
col:=rgbcolorrgb(0,0,255);
create_avail_lvl_image:=createimageWH(w0,h0);
dimg:=create_avail_lvl_image;
imagefillimage(dimg,bg_img3);
draw_frame(dimg,smfr,nil,0,0,w0-1,slot_sy);
gf_outtext(dimg,w0 div 2,slot_sy div 2,font3,1,1,'Available levels',220);
for i:=0 to MAX_LEVELS-1 do
  begin
  inc(i);
  draw_image_alpha(dimg,small_slot_img,0,i*slot_sy,130);
  draw_frame(dimg,smfr,nil,0,i*slot_sy,slot_sx-1,(i+1)*slot_sy);
  draw_frame(dimg,smfr,nil,slot_sx,i*slot_sy,w0-1-inv_slider^.w,(i+1)*slot_sy);
  if i-1+avail_lvl_offs<MAX_LEVELS then
  if avail_lvl[i-1+avail_lvl_offs]<>-1 then
    begin
    gf_outtext(dimg,slot_sx div 2,i*slot_sy+slot_sy div 2,font3,1,1,to_str(avail_lvl[i-1+avail_lvl_offs]),220);
    gf_outtext(dimg,slot_sx+(w0-slot_sx)div 2,i*slot_sy+slot_sy div 2,font4,1,1,lvl_names[avail_lvl[i-1+avail_lvl_offs]],180);
    end;
  dec(i);
  end;
draw_frame(dimg,smfr,nil,w0-inv_slider^.w,slot_sy+1,w0-1,h0);
end;
function create_pass_lvl_image(w0,h0:integer):pimage;
var i:integer;
  col:longint;
  dimg:pimage;
begin
col:=rgbcolorrgb(0,0,255);
create_pass_lvl_image:=createimageWH(w0,h0);
dimg:=create_pass_lvl_image;
imagefillimage(dimg,bg_img3);
draw_frame(dimg,smfr,nil,0,0,w0-1,slot_sy);
gf_outtext(dimg,w0 div 2,slot_sy div 2,font3,1,1,'Finished Levels',220);
for i:=0 to MAX_LEVELS-1 do
  begin
  inc(i);
  draw_image_alpha(dimg,small_slot_img,0,i*slot_sy,130);
  draw_frame(dimg,smfr,nil,0,i*slot_sy,slot_sx-1,(i+1)*slot_sy);
  draw_frame(dimg,smfr,nil,slot_sx,i*slot_sy,w0-1-inv_slider^.w,(i+1)*slot_sy);
  if i-1+pass_lvl_offs<MAX_LEVELS then
  if pass_lvl[i-1+pass_lvl_offs]<>-1 then
    begin
    gf_outtext(dimg,slot_sx div 2,i*slot_sy+slot_sy div 2,font3,1,1,to_str(pass_lvl[i-1+pass_lvl_offs]),220);
    gf_outtext(dimg,slot_sx+(w0-slot_sx)div 2,i*slot_sy+slot_sy div 2,font4,1,1,lvl_names[pass_lvl[i-1+pass_lvl_offs]],180);
    end;
  dec(i);
  end;
draw_frame(dimg,smfr,nil,w0-inv_slider^.w,slot_sy+1,w0-1,h0);
end;

procedure create_scr_img_onstation(scr_img:pimage);
var col:longint;
begin
col:=rgbcolorrgb(0,0,255);
fillimage(scr_img,TRANSPARENCY_COLOR);
if img_update[1] then
  begin
  destroyimage(img_part[1]);
  img_part[1]:=create_player_info_image(mainframe^.w4,mainframe^.w1,getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[1],mainframe^.w4,mainframe^.w1);
if img_update[2] then
  begin
  destroyimage(img_part[2]);
  img_part[2]:=create_ship_info_image(getmaxx div 4+mainframe^.w4,mainframe^.w1,getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[2],getmaxx div 4+mainframe^.w4,mainframe^.w1);
if img_update[3] then
  begin
  destroyimage(img_part[3]);
  img_part[3]:=create_inventory_image(getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[3],getmaxx div 2+mainframe^.w4,mainframe^.w1);
if img_update[4] then
  begin
  destroyimage(img_part[4]);
  img_part[4]:=create_warehouse_image(getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[4],getmaxx*3 div 4+mainframe^.w4,mainframe^.w1);
if img_update[5] then
  begin
  destroyimage(img_part[5]);
  img_part[5]:=create_avail_lvl_image(getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[5],mainframe^.w4,getmaxy div 2+mainframe^.w1);
if img_update[6] then
  begin
  destroyimage(img_part[6]);
  img_part[6]:=create_pass_lvl_image(getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[6],getmaxx div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1);
if img_update[7] then
  begin
  destroyimage(img_part[7]);
  img_part[7]:=create_level_info_image(getmaxx*2 div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1,getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[7],getmaxx*2 div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1);
if img_update[8] then
  begin
  destroyimage(img_part[8]);
  img_part[8]:=create_menu_image(getmaxx*3 div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1,getmaxx div 4-mainframe^.w4-mainframe^.w2+2,getmaxy div 2-mainframe^.w1-mainframe^.w3+2);
  end;
composeimagec(scr_img,img_part[8],getmaxx*3 div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1);
draw_button(ware_slider);
draw_frame(scr_img,mainframe,framemask,0,0,getmaxx div 4,getmaxy div 2);
draw_frame(scr_img,mainframe,framemask,getmaxx div 4,0,getmaxx div 2,getmaxy div 2);
draw_button(inv_slider);
draw_frame(scr_img,mainframe,framemask,getmaxx div 2,0,getmaxx*3 div 4,getmaxy div 2);
draw_frame(scr_img,mainframe,framemask,getmaxx*3 div 4,0,getmaxx,getmaxy div 2);
draw_button(avlvl_slider);
draw_frame(scr_img,mainframe,framemask,0,getmaxy div 2,getmaxx div 4,getmaxy);
draw_button(palvl_slider);
draw_frame(scr_img,mainframe,framemask,getmaxx div 4,getmaxy div 2,getmaxx div 2,getmaxy);
draw_frame(scr_img,mainframe,framemask,getmaxx div 2,getmaxy div 2,getmaxx*3 div 4,getmaxy);
draw_frame(scr_img,mainframe,framemask,getmaxx*3 div 4,getmaxy div 2,getmaxx,getmaxy);
draw_button(view_enemy_Lbut);
draw_button(view_enemy_Rbut);
if skill_points>0 then
  begin
  if SKMAX_MOVESPEED>plpilot^.sk.move_speed then
  	begin
    draw_button(attr_inc_but1);
    attr_inc_but1^.state:=BTS_NORMAL;
    end;
    begin
  	draw_button(attr_inc_but2);
    attr_inc_but2^.state:=BTS_NORMAL;
    end;
    begin
  	draw_button(attr_inc_but3);
    attr_inc_but3^.state:=BTS_NORMAL;
    end;
    begin
  	draw_button(attr_inc_but4);
    attr_inc_but4^.state:=BTS_NORMAL;
    end;
  end
else
	begin
  attr_inc_but1^.state:=BTS_DISABLED;
  attr_inc_but2^.state:=BTS_DISABLED;
  attr_inc_but3^.state:=BTS_DISABLED;
  attr_inc_but4^.state:=BTS_DISABLED;
  end;
draw_button(shipattr1but);
if SKMAX_MOVESPEED>ship_type[plship^.type_]^.sk.move_speed then draw_button(shipattr2but);
draw_button(shipattr3but);
draw_button(shipattr4but);
draw_button(shipattr5but);
draw_button(menu_bt_quit);
draw_button(menu_bt_save);
draw_button(menu_bt_load);
draw_button(menu_bt_newgame);
draw_button(menu_bt_controls);
if viewing_item then
  composeimagec(scr_img,item_view_img,getmaxx div 2-getimagewidth(item_view_img)div 2,getmaxy div 2-getimageheight(item_view_img)div 2);
with cursor do if visible then
  putimage_wmask(scr_img,img^,mask^,mx+dx,my+dy,255);
if show_fps then draw_fps(scr_img);
if console then draw_console(scr_img);
end;

function mouse_click(button:integer):boolean;
begin
mouse_click:=(pmb=button)and(mb=0);
end;

procedure ppitem_array_compress(var ar:array of PPitem;len:integer);
var i,j,kol:integer;
begin
for j:=1 to len do
for i:=0 to len-2 do
	if ar[i]=nil then
  	begin
    ar[i]:=ar[i+1];
    ar[i+1]:=nil;
    end;
end;
procedure ppitem_array_sort_by_cost(var ar:array of PPitem;len:integer);
var i,j:integer;
begin
for i:=1 to len-1 do
for j:=len-1 downto i do
  if (ar[j]<>nil)and(ar[j-1]<>nil)
    and(calc_item_cost(ar[j]^)<calc_item_cost(ar[j-1]^))then swap_pointer(ar[j],ar[j-1]);
end;

function ware_kol_items:integer;
var i:integer;
begin
ware_kol_items:=0;
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do
  if (ware_itm[i]<>nil)and(ware_itm[i]^<>nil) then inc(ware_kol_items);
end;
function inv_kol_items:integer;
var i:integer;
begin
inv_kol_items:=0;
for i:=0 to MAX_INVENTORY_ITEMS-1 do
  if (inv[i]<>nil)and(inv[i]^<>nil) then inc(inv_kol_items);
end;
function kol_avail_levels:integer;
var i:integer;
begin
kol_avail_levels:=0;
for i:=0 to MAX_LEVELS-1 do if avail_lvl[i]<>-1 then inc(kol_avail_levels);
end;
function kol_pass_levels:integer;
var i:integer;
begin
kol_pass_levels:=0;
for i:=0 to MAX_LEVELS-1 do if pass_lvl[i]<>-1 then inc(kol_pass_levels);
end;

function inventory_add_item(itm:PPitem):boolean;
var i:integer;
begin
inventory_add_item:=true;
i:=0;
while (i<MAX_INVENTORY_ITEMS)and(inv[i]<>nil) do inc(i);
if(i<MAX_INVENTORY_ITEMS)and(inv[i]=nil)then inv[i]:=itm
	else inventory_add_item:=false;
end;
procedure warehouse_add_item(itm:PPitem);
var i:integer;
begin
i:=0;
while (i<MAX_WAREHOUSE_ITEMS)and(ware_itm[i]<>nil) do inc(i);
if(i<MAX_WAREHOUSE_ITEMS)and(ware_itm[i]=nil)then ware_itm[i]:=itm;
end;
procedure inventory_remove_item(itm:PPitem);
var i,indx:integer;
begin
i:=0;
indx:=-1;
for i:=0 to MAX_INVENTORY_ITEMS-1 do if inv[i]=itm then indx:=i;
if indx<>-1 then inv[indx]:=nil;
end;
procedure warehouse_remove_item(itm:PPitem);
var i,indx:integer;
begin
i:=0;
indx:=-1;
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do if ware_itm[i]=itm then indx:=i;
if indx<>-1 then ware_itm[indx]:=nil;
end;
{$include buy_item.inc}
procedure onstation_loop;
var i:integer;
  c:char;
begin
update_mouse_input;
inv_offs:=round(bt_slider_posy(inv_slider)*(inv_kol_items-(getmaxy div 2)div slot_sy+2));
if inv_offs<0 then inv_offs:=0;
ware_offs:=round(bt_slider_posy(ware_slider)*(ware_kol_items-(getmaxy div 2)div slot_sy+2));
if ware_offs<0 then ware_offs:=0;
avail_lvl_offs:=round(bt_slider_posy(avlvl_slider)*(kol_avail_levels-(getmaxy div 2)div slot_sy+2));
if avail_lvl_offs<0 then avail_lvl_offs:=0;
pass_lvl_offs:=round(bt_slider_posy(palvl_slider)*(kol_pass_levels-(getmaxy div 2)div slot_sy+2));
if pass_lvl_offs<0 then pass_lvl_offs:=0;
mouse_slot:=-1;
for i:=0 to MAX_SHIP_SLOTS-1 do
  if (mx-getmaxx div 4-mainframe^.w4>get_slot_x(i,ship_window_w))and(mx-getmaxx div 4-mainframe^.w4<get_slot_x(i,ship_window_w)+slot_sx)and
  (my-mainframe^.w1>get_slot_y(i,ship_window_h))and(my-mainframe^.w1<get_slot_y(i,ship_window_h)+slot_sy)
  then
    if plship^.slot[i].exist then mouse_slot:=i;
mouse_inv_slot:=-1;
for i:=0 to MAX_INVENTORY_ITEMS-1 do
  if (ismouseinarea(getmaxx div 2+mainframe^.w4,mainframe^.w1+slot_sy,getmaxx*3 div 4-mainframe^.w2-inv_slider^.w,getmaxy div 2-mainframe^.w3)<>0)
  and p_ismouseinarea(getmaxx div 2+mainframe^.w4,mainframe^.w1+slot_sy,getmaxx*3 div 4-mainframe^.w2-inv_slider^.w,getmaxy div 2-mainframe^.w3)
  then mouse_inv_slot:=inv_offs+(my-mainframe^.w1-slot_sy) div slot_sy;
mouse_ware_slot:=-1;
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do
  if (ismouseinarea(getmaxx*3 div 4+mainframe^.w4,mainframe^.w1+slot_sy,getmaxx-mainframe^.w2-inv_slider^.w,getmaxy div 2-mainframe^.w3)<>0)
  and p_ismouseinarea(getmaxx*3 div 4+mainframe^.w4,mainframe^.w1+slot_sy,getmaxx-mainframe^.w2-inv_slider^.w,getmaxy div 2-mainframe^.w3)
  then mouse_ware_slot:=ware_offs+(my-mainframe^.w1-slot_sy) div slot_sy;
mouse_av_lvl:=-1;
for i:=0 to MAX_LEVELS-1 do
  if (ismouseinarea(mainframe^.w4,getmaxy div 2+mainframe^.w1+slot_sy,getmaxx div 4-mainframe^.w2-avlvl_slider^.w,getmaxy-mainframe^.w3)<>0)
  and p_ismouseinarea(mainframe^.w4,getmaxy div 2+mainframe^.w1+slot_sy,getmaxx div 4-mainframe^.w2-avlvl_slider^.w,getmaxy-mainframe^.w3)
  then mouse_av_lvl:=(my-getmaxy div 2-mainframe^.w1-slot_sy) div slot_sy;
mouse_pa_lvl:=-1;
for i:=0 to MAX_LEVELS-1 do
  if (ismouseinarea(getmaxx div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1+slot_sy,getmaxx div 2-mainframe^.w2-palvl_slider^.w,getmaxy-mainframe^.w3)<>0)
  and p_ismouseinarea(getmaxx div 4+mainframe^.w4,getmaxy div 2+mainframe^.w1+slot_sy,getmaxx div 2-mainframe^.w2-palvl_slider^.w,getmaxy-mainframe^.w3)
  then mouse_pa_lvl:=(my-getmaxy div 2-mainframe^.w1-slot_sy) div slot_sy;

if mouse_click(1) then
  begin
  //ship slots
  if (mouse_slot<>-1)and
    (mbpx-getmaxx div 4-mainframe^.w4>get_slot_x(mouse_slot,ship_window_w))and(mbpx-getmaxx div 4-mainframe^.w4<get_slot_x(mouse_slot,ship_window_w)+slot_sx)and
    (mbpy-mainframe^.w1>get_slot_y(mouse_slot,ship_window_h))and(mbpy-mainframe^.w1<get_slot_y(mouse_slot,ship_window_h)+slot_sy) then
  with plship^.slot[mouse_slot] do
  if ((mitem<>nil)and(itm<>nil)and(itm^^.type_=mitem^^.type_))
  or(mitem=nil)or((itm=nil)and(mitem<>nil)and(mitem^^.type_=itm_type))then
    begin
    swap_pointer(mitem,itm);
    img_update[2]:=true;
    img_update[3]:=true;
    end;
  //inventory slots
  if mouse_inv_slot<>-1 then
    begin
    if (inv[mouse_inv_slot]=nil)or(mitem=nil) then swap_pointer(mitem,inv[mouse_inv_slot])else
    if inventory_add_item(mitem) then mitem:=nil else
    	swap_pointer(mitem,inv[mouse_inv_slot]);
    ppitem_array_compress(inv,MAX_INVENTORY_ITEMS);
    ppitem_array_sort_by_cost(inv,MAX_INVENTORY_ITEMS);
    img_update[2]:=true;
    img_update[3]:=true;
    end;
  //ware
  if (mouse_ware_slot<>-1)and(mitem<>nil) then
    begin
    inc(money,calc_item_cost(mitem^));
    warehouse_add_item(mitem);
    ppitem_array_sort_by_cost(ware_itm,MAX_WAREHOUSE_ITEMS);
    mitem:=nil;
    img_update[1]:=true;
    img_update[2]:=true;
    img_update[4]:=true;
    end else
  if (mouse_ware_slot<>-1)and(mitem=nil)and(ware_itm[mouse_ware_slot]<>nil) then
    begin
    buy_item_init(ware_itm[mouse_ware_slot],getmaxx*3 div 4+mainframe^.w4,(mouse_ware_slot-ware_offs+1)*slot_sy);
    end;
  update_mouse_pointer;
  end;//replacing items
if (mb=2)and(pmb=0)then
  begin
  view_itm:=nil;
  if (mouse_slot<>-1)then view_itm:=plship^.slot[mouse_slot].itm else
  if (mouse_inv_slot<>-1)then view_itm:=inv[mouse_inv_slot] else
  if (mouse_ware_slot<>-1)then view_itm:=ware_itm[mouse_ware_slot];
  if view_itm<>nil then
    begin
    viewing_item:=true;
    item_view_img:=create_item_info_image(view_itm^);
    end;
  end;
if mb<>2 then
if viewing_item then
  begin
  viewing_item:=false;
  destroyimage(item_view_img);
  end;

if mouse_click(1)then
  begin
  if (mouse_av_lvl<>-1)and(avail_lvl[mouse_av_lvl]<>-1) then
    begin
    view_enemy_indx:=0;
    selected_lvl:=avail_lvl[mouse_av_lvl];
    img_update[7]:=true;
    load_level(selected_lvl);
    end;
  if (mouse_pa_lvl<>-1)and(pass_lvl[mouse_pa_lvl]<>-1) then
    begin
    view_enemy_indx:=0;
    selected_lvl:=pass_lvl[mouse_pa_lvl];
    img_update[7]:=true;
    load_level(selected_lvl);
    end;
  end;

if keystate[123]=128 then make_screenshot;
if console then while keypressed do
  begin
  c:=readkey;
  case c of
  #0:case readkey of
    #60:console:=false;
    	end;
    #27:console:=false;
    #13:begin
      console:=false;
      console_script[0]:=console_str;
      execute_script(console_script);
      for i:=1 to 8 do img_update[i]:=true;
      end;
    #8:if length(console_str)>1 then console_str:=copy(console_str,1,length(console_str)-1);
    else console_str:=console_str+c;
    end;
  end else
if keypressed then
begin
c:=readkey;
case c of
  '0'..'9':if (mouse_slot<>-1) then plship^.slot[mouse_slot].hotkey:=c;
  '-':if (mouse_slot<>-1)then plship^.slot[mouse_slot].hotkey:=#0;
  #59:begin
  		controls_init(main_loop_proc);
			main_loop_proc:=@controls_loop;
  		end;
  #60:begin
    console:=not console;
    if console then console_str:=' ';
    clear_script(console_script);
    end;
  #63:save_game('saved_games\quicksave');
  #67:load_game('saved_games\quicksave');
  #72:begin
    i:=-1;
    case mx div (getmaxx div 4)+1+4*(my div (getmaxy div 2)) of
      3:;
      4:;
      end;
    end;
  #80:i:=1;
  end;
end;
if main_loop_proc=@onstation_loop then
  begin
  //UPDATE GATE
  if gatetime>gateframetime then
    begin
    gatetime:=0;
    if (gatebutton^.prelighted)and(gateframe<gateframes) then
      begin
      img_update[7]:=true;
      inc(gateframe);
      end;
    if not(gatebutton^.prelighted)and(gateframe>1)then
      begin
      dec(gateframe);
      img_update[7]:=true;
      end;
    end;
  //
  i:=get_mouse_image_part;
  if i<9 then img_update[i]:=true;
  create_scr_img_onstation(scr_img);
  putimage(0,0,scr_img);
  for i:=1 to 8 do img_update[i]:=false;
  //
  gatetime:=gatetime+looptime*1000;//g_timer_elapsed(main_timer,nil)*1000;
  for i:=0 to MAX_BUTTONS-1 do if bt[i]<>nil then update_button(bt[i],pmb,mb,mx,my);
  end;
end;
