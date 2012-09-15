program spaceWarrior;
uses windows,graphix,gxtype,gximg,gxmouse,gxcrt,gximeff,gxdrw,gxtext,gxmedia,
  math,sysutils,bass;
const
  MAX_LAYERS=10;
  MAX_LEVELS=100;

  ITEM_DROP_CHANCE=15;
  CAMERA_SHAKING_POWER=300;
  SILENCE_DIST=1;
  SOUNDSILENCE_DIST=2;
  MSG_STR_STD_TIME=12000;
  CONTAINER_EFF_INDX=4;
  MAX_LEVEL_ENEMY_TYPES=10;
  MAX_LEVEL_ALLY_TYPES=10;
  MAX_LEVEL_ASTEROID_TYPES=10;
  MAX_LEVEL_BACKGROUNDS=5;
  MAX_ITEM_NAME_LEN=30;
  SHIP_MOVE_CONST=600;

  MAX_EVENT_MESSAGES=10;
  MAX_SCRIPT_COMMANDS=10;
  MAX_CONTAINERS=100;

  MAX_ASTEROID_TYPES=15;
  MAX_ASTEROIDS=50;
  MAX_ASTEROID_CHILD_TYPES=10;
  MAX_BULLETS=1500;
  MAX_BULLET_TYPES=10;
  MAX_FLDGENERATORS=10;
  MAX_EFFECTS_TYPE=25;
  MAX_EFFECTS=200;
  MAX_FRAMES=50;
  MAX_PARTICLES=3000;
  MAX_PARTICLE_TYPES=10;
  MAX_SHIP_TYPES=10;
  MAX_SHIPS=200;
  MAX_SHIP_SLOTS=10;
  MAX_PILOT_TYPES=10;
  MAX_ITEMS=1000;

  SKMAX_MOVESPEED=3;

  MAX_INVENTORY_ITEMS=100;
  MAX_WAREHOUSE_ITEMS=100;
  ITM_WEAPON=1;
  ITM_FLD_GEN=2;
  ITM_SCRIPT=3;

  CONTROL_AUTO=1;
  CONTROL_MAN=2;

  MAX_ITEM_MODIFICATIONS=10;
  IM_ADD=0;
  IM_MUL=1;
  IM_MAX=2;
  IMW_DMG=0;
  IMW_DMG1=1;
  IMW_DMG2=2;
  IMW_LIFE=3;
  IMW_MAX=4;
  IMF_KOEF=0;
  IMF_REFL=1;
  IMF_CLD=2;
type
	Tprocedure=procedure;
  charset=set of char;
  PPimage=^pimage;

  Tscript=array[0..MAX_SCRIPT_COMMANDS-1]of string[20];

  Tskill_list=record
    bullet_speed,move_speed,bullet_dmg,attack_speed:double;
    end;
  Tpilot_skill_list=record
    bullet_speed,move_speed,bullet_dmg,merchant:double;
    end;

  Tpilot=record
    sk:Tpilot_skill_list;
    name:string[50];
    end;
  Ppilot=^Tpilot;

  Teff_type=record{frametime in milliseconds}
    img:array[0..MAX_FRAMES-1]of pimage;
    img_mask:array[0..MAX_FRAMES-1]of pimage;
    frametime,kol_fr:integer;
    mask:boolean;
    soundpath:string[50];
    end;
  Peff_type=^Teff_type;

  Teff=record
    x,y,cur_fr,type_:longint;
    time:double;
    cycle:boolean;
    layer:integer;
    end;
  Peff=^Teff;
  PPeff=^Peff;

  PPitem=^Pitem;
  Tshipslot=record
    exist:boolean;
    dx,dy,itm_type:integer;
    hotkey:char;
    itm:PPitem;
    end;

  Tship_slot_init=array[0..MAX_SHIP_SLOTS-1]of
    record
    exist:boolean;
    itm_type,itm_indx,dx,dy:integer;
    hotkey:char;
    end;

  Tship_type=record
    g:array[-5..5]of record
      img,img_mask:pimage;
      end;
    turnab:boolean;
    turnkoef:double;
    eff_indx,score_prize,ptype,life,pilot_indx,att_cooldown:integer;
    sk:Tskill_list;
    slot0:Tship_slot_init;
    mainimgpath,imgmaskpath:string[50];
    end;
  Pship_type=^Tship_type;

  Tship=record
    x,y,tx,ty,att_cooldown:double;
    ai_action_cooldown:longint;
    type_,dir,coff,life,control,bul_refl:integer;
    active,enemy:boolean;
    plt:Ppilot;
    fld,fld_force:double;
    fld_eff:PPeff;
    slot:array[0..MAX_SHIP_SLOTS-1]of Tshipslot;
    slot_cooldown:array[0..MAX_SHIP_SLOTS-1]of double;
    layer:integer;
    end;
  Pship=^Tship;
  PPship=^Pship;

  Titem_type_info=record
    view_img_path:string[50];
    name:string[MAX_ITEM_NAME_LEN];
    img,img_mask:pimage;
    droplvl:longint;
    end;
  Pitem_type_info=^Titem_type_info;
  PPitem_type_info=^Pitem_type_info;

  Tbullet_type=record
    img:pimage;
    eff_indx,vel,w,h,cooldown,ptype,dmg1,dmg2,guide_time,guide_dist,
      lifetime,alpha,cross,maxplife,kolpt:longint;
    maxpoffs,camera_shake_koef:double;
    guided,cr_destroy:boolean;
    soundpath:string[50];
    iti:Titem_type_info;
    end;
  Pbullet_type=^Tbullet_type;

  Tfield_generator_type=record
    cooldown,refl_chance,eff_indx:longint;
    koef:double;
    iti:Titem_type_info;
    end;
  Pfield_generator_type=^Tfield_generator_type;

  Tbullet=record
    x,y,px,py,velx,vely,guide_time,lifetime,dmg_koef:double;
    type_,cross:integer;
    enemy:boolean;
    target,prev_target:PPship;
    layer:integer;
    par:PPitem;
    end;
  Pbullet=^Tbullet;

  Pitem_modification=^Titem_modification;
  Titem_modification=record
    m,param:integer;
    val:double;
    end;

  Titem=record
    type_,indx:integer;
    modif:array[0..MAX_ITEM_MODIFICATIONS-1]of Pitem_modification;
    end;
  Pitem=^Titem;

  Tparticle_type=record
    w,h:integer;
    img,img_mask:pimage;
    end;
  Pparticle_type=^Tparticle_type;

  Tparticle=record
    x,y,velx,vely,life,maxlife,alpha:double;
    type_:integer;
    layer:integer;
    end;
  Pparticle=^Tparticle;

  Tcontainer=record
    ef:PPeff;
    picked:boolean;
    x,y:double;
    sc:Tscript;
    end;
  Pcontainer=^Tcontainer;

  Tasteroid_type=record
    eff_indx,deatheff_indx,layer,w,h,dmg_perc,childmin,childmax,
      child_kol_types:integer;
    child:array[0..MAX_ASTEROID_CHILD_TYPES-1]of integer;
    camera_shake_koef:double;
    end;
  Pasteroid_type=^Tasteroid_type;

  Tasteroid=record
    type_:integer;
    velx,vely,accx,accy,x,y:double;
    eff:PPeff;
    end;
  Pasteroid=^Tasteroid;
  PPasteroid=^Pasteroid;

{$include inc\sound.inc}
{$include inc\vectors.inc}
{$include inc\keys.inc}
procedure setvideomode(resx,resy,kol_colors:longint);
begin
case kol_colors of
  32:setmodegraphix(resx,resy,ig_col32);
  24:setmodegraphix(resx,resy,ig_col24);
  16:setmodegraphix(resx,resy,ig_col16);
  15:setmodegraphix(resx,resy,ig_col15);
  8:setmodegraphix(resx,resy,ig_col8);
  end;
bar(0,0,getmaxx+1,getmaxy+1,rgbcolorrgb(0,0,0));
end;

function getclocks : Float;
  var
    t, f : large_integer;
  begin
    QueryPerformanceCounter(t);
    QueryPerformanceFrequency(f);
    getclocks:=Comp(t)/Comp(f);
  end;
function getkeyboardstate(lpKeyState:Pbyte):longbool;external 'user32' name 'GetKeyboardState';
var
	main_sound:Psound;
	logfile:text;
  prevclocks,looptime,gamespeedkoef,lt_input,lt_crimg,lt_draw,lt_bullets,
  	lt_eff,lt_part,lt_asteroids,lt_cont,lt_plship,lt_ships,lt_other,
    ltgr_other:float;
  ltgr_layer:array[0..MAX_LAYERS-1]of float;
  keystate:array[0..255]of byte;
  mainloop_report:boolean;
  mbpx,mbpy,mx,my,pmx,pmy,mb,pmb:longint;
  scr_img:pimage;
  i,resx,resy,kol_colors,TRANSPARENCY_COLOR,ast_cooldown,
    soundvol:longint;
  cam_shake_vel,cam_shake_dvec,cam_shake_force:Tvec;
  main_end,show_hp,game_paused,player_auto_repair,cam_shake,
    show_statpanel,show_item_panel,show_fps:boolean;
  switch_time:longint;
  ev_msg:array[0..MAX_EVENT_MESSAGES-1]of record
    exist:boolean;
    text:ansistring;
    life:double;{in milliseconds}
    end;
  main_loop_proc:procedure;
  cursor:record
    img,mask:PPimage;
    dimg,dmask:pimage;
    visible:boolean;
    dx,dy:integer;
    end;
  font1path,font2path,font3path,font4path:ansistring;
  panel:record
    img:pimage;
    x,y1,y2,y3,y4,w,h:integer;
    end;
  itempanel:record
    img_upper,img_lower,img_center,img_full:pimage;
    barx,bary,barh,barw,cut_dy,itmx,itmy:integer;
    end;
  //level
  lvl_screens,lvl_enemies,lvl_bosstype,lvl_cont_prob,cur_lvl_uin,
    lvl_ast_prob,lvl_kol_ast_types,lvl_ast_max_cooldown:integer;
  lvl_bg:array[0..MAX_LEVEL_BACKGROUNDS-1]of record
  	img:pimage;
    dist:double;
    path:ansistring;
    alpha:integer;
  	end;
  lvl_ast_minvelx,lvl_ast_minvely,
    lvl_ast_velxoffs,lvl_ast_velyoffs:double;
  lvl_ast_types:array[0..MAX_LEVEL_ASTEROID_TYPES-1]of integer;
  lvl_enemy_types:array[0..MAX_LEVEL_ENEMY_TYPES-1]of record
    type_,kol:integer;
    end;
  lvl_ally_types:array[0..MAX_LEVEL_ALLY_TYPES-1]of record
    type_,kol:integer;
    end;
  boss_ship:PPship;
  lvl_accomplished:boolean;
  lvl_finish_script:Tscript;
  level_finish_check_proc:function:boolean;
  //
  bullet:array[0..MAX_BULLETS-1]of Pbullet;
  bullet_type:array[0..MAX_BULLET_TYPES-1]of Pbullet_type;
  fld_gen:array[0..MAX_FLDGENERATORS-1]of Pfield_generator_type;
  eff_type:array[0..MAX_EFFECTS_TYPE-1]of Peff_type;
  eff:array[0..MAX_EFFECTS-1]of Peff;
  ship:array[0..MAX_SHIPS-1]of Pship;
  ship_type:array[0..MAX_SHIPS-1]of Pship_type;
  pt:array[0..MAX_PARTICLES-1]of Pparticle;
  ptt:array[0..MAX_PARTICLE_TYPES-1]of Pparticle_type;
  pilot:array[0..MAX_PILOT_TYPES-1]of Ppilot;
  item:array[0..MAX_ITEMS-1]of Pitem;
  cont:array[0..MAX_CONTAINERS-1]of Pcontainer;
  ast_type:array[0..MAX_ASTEROID_TYPES-1]of Pasteroid_type;
  ast:array[0..MAX_ASTEROIDS-1]of Pasteroid;
  //player
  avail_lvl:array[0..MAX_LEVELS-1]of integer;
  pass_lvl:array[0..MAX_LEVELS-1]of integer;
  lvl_names:array[0..MAX_LEVELS-1]of string[20];
  plship:Pship;
  plpilot:Ppilot;
  scr_y:double;
  scr_x:longint;
  exp,next_exp,cur_exp,pllvl,skill_points,money,repair_kol:longint;
  lvl_exp_collected:longint;
  player_lose:boolean;
  inv:array[0..MAX_INVENTORY_ITEMS-1]of PPitem;
  ware_itm:array[0..MAX_WAREHOUSE_ITEMS-1]of PPitem;
  //
  hp_sp:double;
  hp_sp_dir,hp_sp_bl:integer;
  //

procedure add_event_msg(text0:ansistring;life0:longint);
var i,j:integer;
begin
i:=0;
while (i<MAX_EVENT_MESSAGES)and(ev_msg[i].exist) do inc(i);
if i>=MAX_EVENT_MESSAGES then
  begin
  for j:=0 to MAX_EVENT_MESSAGES-2 do ev_msg[j]:=ev_msg[j+1];
  i:=MAX_EVENT_MESSAGES-1;
  end;
with ev_msg[i] do
  begin
  text:=text0;
  life:=life0;
  exist:=true;
  end
end;

procedure swap_int(var a,b:integer);forward;
procedure finish_current_level;forward;
function p_ismouseinarea(x1,y1,x2,y2:integer):boolean;
begin
if x1>x2 then swap_int(x1,x2);
if y1>y2 then swap_int(y1,y2);
p_ismouseinarea:=(mbpx>x1)and(mbpx<x2)and(mbpy>y1)and(mbpy<y2);
end;
procedure update_mouse_input;
begin
pmx:=mx;pmy:=my;
mousecoords(mx,my);
pmb:=mb;
mb:=mousebutton;
if (pmb=0)and(mb<>0) then
  begin
  mbpx:=mx;
  mbpy:=my;
  end;
end;
procedure new_game;forward;
procedure load_level(lvl_uin:integer);forward;
procedure prepare_level;forward;
procedure destroy_bullet(var b:Pbullet;cr_eff:boolean);forward;
procedure new_bullet(x0,y0:double;type0,dir:integer;enemy0:boolean;vel_koef,dmg_koef0:double;layer0:integer;par0:PPitem);forward;
procedure new_particle(x0,y0,life0,velx0,vely0:double;type0:integer;layer0:integer);forward;
function bullet_find_target(b:Pbullet):PPship;forward;
function new_effect(x0,y0,type0:longint;cycle0:boolean;layer0:integer):PPeff;forward;
procedure destroy_effect(var ef:Peff);forward;
function eff_w(ef:Peff):integer;forward;
function eff_h(ef:Peff):integer;forward;
function ship_w(sh:Pship):longint;forward;
function ship_h(sh:Pship):longint;forward;
function inventory_add_item(itm:PPitem):boolean;forward;
procedure inventory_remove_item(itm:PPitem);forward;
procedure onstation_loop;forward;
procedure onstation_quit(save:boolean);forward;
procedure onstation_init(update_warehouse,save:boolean);forward;
procedure clear_all;forward;
function ship_new(x0,y0,type0,dir0:longint;enemy0:boolean;control0:byte;layer0:integer):PPship;forward;
procedure destroy_pilot(var plt:Ppilot);forward;
procedure destroy_ship_type(var sht:Pship_type);forward;
procedure load_pilot_type(var f:text);forward;
procedure load_ship_type(var f:text);forward;
procedure warehouse_add_item(itm:PPitem);forward;
procedure save_main_settings(path:ansistring);forward;
//
function level_finished_boss:boolean;
begin
level_finished_boss:=boss_ship^=nil;
end;
function level_finished_wo_boss:boolean;
var i,kol:integer;
begin
kol:=0;
for i:=0 to MAX_SHIPS-1 do if (ship[i]<>nil)and(ship[i]^.enemy)then inc(kol);
level_finished_wo_boss:=kol=0;
end;

{$include inc\intro.inc}
{$include inc\cam_shake.inc}
{$include inc\player.inc}
{$include inc\other.inc}
{$include inc\graph.inc}
var smfr,mainframe,framemask:Pframe;
{$include inc\buttons.inc}
{$include inc\controls.inc}
{$include inc\options.inc}
{$include inc\items.inc}
{$include inc\end_game.inc}
{$include inc\script.inc}
{$include inc\file_select.inc}
{$include inc\save_load.inc}
{$include inc\effects_ships_bullets.inc}
procedure level_loop;forward;
{$include inc\gamemenu.inc}
{$include inc\onstation.inc}
{$include inc\game_process.inc}
{$include inc\level_io.inc}
{$include inc\cfg_load.inc}

procedure clear_all;
begin
randomize;
for i:=0 to MAX_INVENTORY_ITEMS-1 do inv[i]:=nil;
for i:=0 to MAX_WAREHOUSE_ITEMS-1 do ware_itm[i]:=nil;
for i:=0 to MAX_ITEMS-1 do destroy_item(item[i]);
for i:=0 to MAX_EFFECTS-1 do destroy_effect(eff[i]);
for i:=0 to MAX_SHIPS-1 do destroy_ship(ship[i],false);
for i:=0 to MAX_BULLETS-1 do destroy_bullet(bullet[i],false);
for i:=0 to MAX_PARTICLES-1 do destroy_particle(pt[i]);
for i:=0 to MAX_CONTAINERS-1 do destroy_container(cont[i]);
for i:=0 to MAX_ASTEROIDS-1 do destroy_asteroid(ast[i]);
for i:=0 to MAX_LEVELS-1 do avail_lvl[i]:=-1;
for i:=0 to MAX_LEVELS-1 do pass_lvl[i]:=-1;
end;

procedure new_game;
var i:longint;
begin
clear_all;
skill_points:=0;
pllvl:=1;
next_exp:=get_exp(2);
exp:=0;
cur_exp:=0;
money:=0;
repair_kol:=3;
player_auto_repair:=false;
show_statpanel:=true;
show_item_panel:=true;
show_fps:=false;
avail_lvl[0]:=1;
plship:=ship_new(getmaxx div 2,getmaxy div 20,0,1,false,CONTROL_MAN,3)^;
plpilot:=plship^.plt;
onstation_init(true,false);
main_loop_proc:=@onstation_loop;
end;

begin
i:=paramcount;
while i>0 do
  begin
  if paramstr(i)='-r' then mainloop_report:=true;
  dec(i);
  end;
load_main_settings('config.cfg');
assign(logfile,'logfile.log');
rewrite(logfile);
main_end:=false;
initgraphix(ig_lfb,ig_vesa);
setvideomode(resx,resy,kol_colors);
gf_reset_colors;
TRANSPARENCY_COLOR:=rgbcolorrgb(0,0,0);
scr_img:=createimageWH(getmaxx+1,getmaxy+1);
initmouse;
disablemouse;
//
load_all_files('files.txt');
font1path:='gfx\font.gif';
font2path:='gfx\font2.gif';
font3path:='gfx\font3.gif';
font4path:='gfx\font4.gif';
gf_fontload(font1path,font1);
gf_fontload(font2path,font2);
gf_fontload(font3path,font3);
gf_fontload(font4path,font4);
load_frame(smfr,'gfx\other\frame_small.gif');
load_frame(mainframe,'gfx\other\frame.gif');
with cursor do
  begin
  loadimagefile(itdetect,'gfx/cursor.gif',dimg,0);
  loadimagefile(itdetect,'gfx/cursor.gif',dmask,2);
  img:=@dimg;
  mask:=@dmask;
  dx:=0;dy:=0;
  end;
load_panel('panel.txt');
load_itempanel('item_panel.txt');
sound_destroysounds;
play_intro('intro.txt');
bass_setconfig(bass_config_gvol_stream,soundvol);
main_sound:=nil;
new_game;
repeat
  begin
  looptime:=getclocks-prevclocks;
  if looptime>1 then looptime:=0;
  if game_paused then looptime:=0;
  prevclocks:=getclocks;
  getkeyboardstate(@keystate);
  for i:=0 to 255 do if keystate[i]=129 then keystate[i]:=128;
  main_loop_proc();
  sound_checksounds;
  end
until main_end;

bass_free;
sound_destroysounds;
destroy_frame(mainframe);
destroy_frame(framemask);
destroy_frame(smfr);
donegraphix;
close(logfile);
end.
