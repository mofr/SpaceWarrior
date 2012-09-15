procedure compose_layer(scr_img:pimage;layer0,vdx,vdy:integer);
var timg1,timg2:pimage;
  i,j,dx,dy,t:integer;
  k:double;
  color:longint;
begin
for i:=0 to MAX_BULLETS-1 do if bullet[i]<>nil then
  with bullet[i]^ do
  if layer=layer0 then
  begin
  timg1:=cloneimage(bullet_type[type_]^.img);
  dx:=0;
  dy:=0;
  if (velx<>0) then
    begin
    timg2:=timg1;
    dx:=getimagewidth(timg2);
    dy:=getimageheight(timg2);
    if dx>dy then t:=dx else t:=dy;
    timg1:=createimageWH(t*2,t*2);
    setimagetransparencycolor(timg1,rgbcolorrgb(255,255,255));
    fillimage(timg1,getimagetransparencycolor(timg1));
    k:=arccos(velx/sqrt(sqr(velx)+sqr(vely)))*180/pi;
    k:=k+360-90;
    if k>360 then k:=k-360;
    if k<>0 then k:=k/360 else k:=360;
    rotateimage(timg1,timg2,t div 2,t div 2,dx div 2,dy div 2,dx,dy,round(65535*k));
    if vely<0 then flipimageV(timg1,timg1);
    destroyimage(timg2);
    dx:=-t div 2+bullet_type[type_]^.w div 2-round(bullet_type[type_]^.h*cos((k*360+90)*pi/180));
    dy:=-t div 2-bullet_type[type_]^.h div 2+sign(vely)*round(bullet_type[type_]^.h*{}sin((k*360+90)*pi/180){});
    end
  else if vely<0 then
    begin
    dy:=-bullet_type[type_]^.h;
    flipimageV(timg1,timg1);
    end;
  draw_image_alpha(scr_img,timg1,round(x)-bullet_type[type_]^.w div 2+dx+vdx,getmaxy-round(y-scr_y)+dy+vdy,bullet_type[type_]^.alpha);
  destroyimage(timg1);
  end;
for i:=0 to MAX_SHIPS-1 do if ship[i]<>nil then
  if ship_visible(ship[i],0)then
  with ship[i]^ do
  if layer=layer0 then
  begin
  timg1:=cloneimage(ship_type[type_]^.g[coff].img);
  timg2:=cloneimage(ship_type[type_]^.g[coff].img_mask);
  j:=0;
  if dir=-1 then
    begin
    flipimageV(timg1,timg1);
    flipimageV(timg2,timg2);
    j:=ship_h(ship[i]);
    end;
  if show_hp and(plship<>ship[i]) then
  	begin
    if enemy then color:=rgbcolorrgb(100,100,250) else color:=rgbcolorrgb(250,100,100);
    gf_bar(scr_img,round(x)+vdx,getmaxy-round(y-scr_y+j)-5+vdy,ship_w(ship[i]),3,false,(life)/(ship_type[type_]^.life),0,color,150);
    end;
  putimage_wmask(scr_img,timg1,timg2,round(x)+vdx,getmaxy-round(y-scr_y+j)+vdy,255);
  destroyimage(timg1);
  destroyimage(timg2);
  end;
for i:=0 to MAX_PARTICLES-1 do if pt[i]<>nil then
  with pt[i]^ do
  if layer=layer0 then
  begin
  putimage_wmask(scr_img,ptt[type_]^.img,ptt[type_]^.img_mask,round(x-ptt[type_]^.w div 2)+vdx,getmaxy-round(y-scr_y+ptt[type_]^.h div 2)+vdy,round(alpha));
  end;
for i:=0 to MAX_EFFECTS-1 do if eff[i]<>nil then
  with eff[i]^ do
  if layer=layer0 then
  begin
  timg1:=cloneimage(eff_type[type_]^.img[cur_fr]);
  if eff_type[type_]^.mask then
    begin
    timg2:=cloneimage(eff_type[type_]^.img_mask[cur_fr]);
    putimage_wmask(scr_img,timg1,timg2,x-getimagewidth(timg1)div 2+vdx,getmaxy-round(y+getimageheight(timg1)div 2-scr_y)+vdy,255);
    destroyimage(timg2);
    end
  else
    composeimagec(scr_img,timg1,x-getimagewidth(timg1)div 2+vdx,getmaxy-round(y+getimageheight(timg1)div 2-scr_y)+vdy);
  destroyimage(timg1);
  end;
end;

procedure create_scr_img_gameprocess(scr_img:pimage);
var i,j,dx,dy,t,x_,y_:integer;
  k,lt:double;
  timg1,timg2:pimage;
  s:ansistring;
begin
ltgr_other:=0;
lt:=getclocks;
fillimage(scr_img,TRANSPARENCY_COLOR); 
for t:=0 to MAX_LEVEL_BACKGROUNDS-1 do
if lvl_bg[t].dist<>-1 then
for i:=-1 to getmaxx div getimagewidth(lvl_bg[t].img)+1 do
for j:=-1 to getmaxy div getimageheight(lvl_bg[t].img) do
	begin
	x_:=i*getimagewidth(lvl_bg[t].img)-round(scr_x/lvl_bg[t].dist)+round(cam_shake_dvec.x);
	y_:=j*getimageheight(lvl_bg[t].img)+round(scr_y/lvl_bg[t].dist)mod getimageheight(lvl_bg[t].img)+round(cam_shake_dvec.y);
	if((x_<=getimagewidth(scr_img))and(x_>=0))or
		((x_+getimagewidth(lvl_bg[t].img)<=getimagewidth(scr_img))and(x_+getimagewidth(lvl_bg[t].img)>=0))or
		((x_<=0)and(x_+getimagewidth(lvl_bg[t].img)>=getimagewidth(scr_img)))or
		((y_<=getimageheight(scr_img))and(y_>=0))or
		((y_+getimageheight(lvl_bg[t].img)<=getimageheight(scr_img))and(y_+getimageheight(lvl_bg[t].img)>=0))or
		((y_<=0)and(y_+getimageheight(lvl_bg[t].img)>=getimageheight(scr_img)))
		then
  composeimagec(scr_img,lvl_bg[t].img,x_,y_);
  end;
ltgr_other:=ltgr_other+getclocks-lt;lt:=getclocks;

for i:=0 to MAX_LAYERS-1 do
	begin
	compose_layer(scr_img,i,round(cam_shake_dvec.x),round(cam_shake_dvec.y));
	ltgr_layer[i]:=getclocks-lt;lt:=getclocks;
	end;
if plship<>nil then
with panel do
if show_statpanel then
  begin
  composeimagec(scr_img,img,0,getmaxy-getimageheight(img)+2);
  gf_bar(scr_img,x,getmaxy-getimageheight(img)+y2-h div 2,w,h,true,plship^.fld,rgbcolorrgb(40,40,150),rgbcolorrgb(100,100,250),160);
  gf_bar(scr_img,x,getmaxy-getimageheight(img)+y3-h div 2,w,h,true,(exp-cur_exp)/(next_exp-cur_exp),rgbcolorrgb(100,100,20),rgbcolorrgb(250,250,100),160);
  if plship<>nil then with ship_type[plship^.type_]^ do
    begin
    k:=(plship^.life)/(life);
    gf_bar(scr_img,x,getmaxy-getimageheight(img)+y1-h div 2,w,h,true,k
      ,rgbcolorrgb(100,20,20),rgbcolorrgb(250,100-round(hp_sp),100-round(hp_sp)),160-round(hp_sp*130/100));
    end;
  s:=to_str(repair_kol);
  if player_auto_repair then s:=s+' auto';
  gf_outtext(scr_img,x,getmaxy-getimageheight(img)+y4,font4,0,1,s,180);
  end
else
  begin
  if plship<>nil then with ship_type[plship^.type_]^ do
    begin
    k:=(plship^.life)/(life);
    gf_bar(scr_img,5,getmaxy-5-h-h-2,w,h,true,k
      ,rgbcolorrgb(100,20,20),rgbcolorrgb(250,100-round(hp_sp),100-round(hp_sp)),160-round(hp_sp*130/100));
    k:=plship^.fld;
    gf_bar(scr_img,5,getmaxy-5-h,w,h,true,k
      ,rgbcolorrgb(40,40,150),rgbcolorrgb(100,100,250),160);
    end;
  end;
///Item panel
t:=0;
if plship<>nil then
for i:=0 to MAX_SHIP_SLOTS-1 do
if(plship^.slot[i].exist)and(plship^.slot[i].itm<>nil)
  and((plship^.slot[i].itm^^.type_=ITM_WEAPON)or(plship^.slot[i].itm^^.type_=ITM_FLD_GEN))
  then inc(t);
if t<2 then dy:=getimageheight(itempanel.img_full) else
  begin
  dy:=getimageheight(itempanel.img_upper)+getimageheight(itempanel.img_center);
  if t>2 then dy:=dy+(t-2)*getimageheight(itempanel.img_center);
  end;
dy:=getmaxy-dy;
j:=0;
if (plship<>nil)and(show_item_panel) then
with itempanel do
for i:=0 to MAX_SHIP_SLOTS-1 do
if (plship^.slot[i].exist)and(plship^.slot[i].itm<>nil)
  and((plship^.slot[i].itm^^.type_=ITM_WEAPON)or(plship^.slot[i].itm^^.type_=ITM_FLD_GEN))then
  begin
  y_:=dy;
  if (j=0)and(j=t-1)then
    begin
    x_:=getmaxx-getimagewidth(img_full);
    composeimagec(scr_img,img_full,x_,y_);
    dy:=dy+getimageheight(img_full);
    end
  else
  if j=0 then
    begin
    x_:=getmaxx-getimagewidth(img_upper);
    composeimagec(scr_img,img_upper,x_,y_);
    dy:=dy+getimageheight(img_upper);
    end
  else
  if j=t-1 then
    begin
    x_:=getmaxx-getimagewidth(img_lower);
    composeimagec(scr_img,img_lower,x_,y_);
    dy:=dy+getimageheight(img_lower);
    y_:=y_-(getimageheight(img_full)-getimageheight(img_lower));
    end
  else
    begin
    x_:=getmaxx-getimagewidth(img_center);
    composeimagec(scr_img,img_center,x_,y_);
    dy:=dy+getimageheight(img_center);
    y_:=y_-(getimageheight(img_full)-getimageheight(img_upper));
    end;
  k:=0;
  case plship^.slot[i].itm^^.type_ of
    ITM_WEAPON:begin
      k:=(plship^.slot_cooldown[i])/(bullet_type[plship^.slot[i].itm^^.indx]^.cooldown);
      timg1:=bullet_type[plship^.slot[i].itm^^.indx]^.iti.img;
      timg2:=cloneimage(bullet_type[plship^.slot[i].itm^^.indx]^.iti.img_mask);
      end;
    ITM_FLD_GEN:begin
      k:=(plship^.slot_cooldown[i])/(fld_gen[plship^.slot[i].itm^^.indx]^.cooldown);
      timg1:=fld_gen[plship^.slot[i].itm^^.indx]^.iti.img;
      timg2:=cloneimage(fld_gen[plship^.slot[i].itm^^.indx]^.iti.img_mask);
      end;
    end;
  if k>1 then k:=1;
  if k<0 then k:=0;
  imagebar(scr_img,x_+barx,y_+bary+barh,x_+barx+barw,y_+bary+round(barh*k),rgbcolorrgb(round(255*k),round(255*(1-k)),0));
//  if k>0 then imageSSUBcolor(timg2,timg2,rgbcolorrgb(150,150,150));
  putimage_wmask(scr_img,timg1,timg2,x_+itmx-getimagewidth(timg1)div 2,y_+itmy-getimageheight(timg1)div 2,255);
  destroyimage(timg2);
  inc(j);
  end;
//Event messages
j:=0;
x_:=getmaxx-getmaxy div 50;
if show_item_panel then x_:=x_-getimagewidth(itempanel.img_full);
for i:=MAX_EVENT_MESSAGES-1 downto 0 do if ev_msg[i].exist then
  begin
  t:=round(25*ev_msg[i].life/1000);
  if t>255 then t:=255;
  gf_outtext(scr_img,x_,getmaxy-(font3^.h+font3^.dl)*j-15,font3,2,2,ev_msg[i].text,t);
  inc(j);
  end;
if player_lose{ and(switch_time<3000)} then
  begin
  gf_outtext(scr_img,getmaxx div 2,getmaxy div 2-font1^.dl,font1,1,2,'GAME OVER',100);
  gf_outtext(scr_img,getmaxx div 2,getmaxy div 2+font1^.dl,font1,1,0,'PRESS ENTER TO CONTINUE',100);
  end else
if lvl_accomplished{ and(switch_time<3000)} then
  begin
  gf_outtext(scr_img,getmaxx div 2,getmaxy div 2-font1^.dl,font1,1,2,'LEVEL ACCOMPLISHED',100);
  gf_outtext(scr_img,getmaxx div 2,getmaxy div 2+font1^.dl,font1,1,0,'PRESS ENTER TO CONTINUE',100);
  end;
if game_paused then gf_outtext(scr_img,getmaxx div 2,getmaxy*28 div 30,font2,1,2,'Game Paused',180);
ltgr_other:=ltgr_other+getclocks-lt;lt:=getclocks;
with cursor do if visible then
  putimage_wmask(scr_img,img^,mask^,mx,my,255);
if show_fps then draw_fps(scr_img);
end;

procedure player_repair(check:boolean);
begin
if (plship<>nil)and(not check or (repair_kol>0)) then
  begin
  dec(repair_kol);
  plship^.life:=plship^.life+round(ship_type[plship^.type_]^.life*0.3);
  if plship^.life>ship_type[plship^.type_]^.life then
    plship^.life:=ship_type[plship^.type_]^.life;
  end;
end;

procedure level_loop;
var i,t:longint;
  time,lt:double;
  c:char;
label end_loop;
begin
lt_other:=0;
lt:=getclocks;
update_mouse_input;
if (keystate[VK_F12]=128)or(keystate[VK_SNAPSHOT]=128) then make_screenshot;
if (keystate[192]=128)or(keystate[VK_CONTROL]=128) then ship_attack(plship,-1);
if plship<>nil then
for i:=0 to 9 do
if (keystate[48+i]=128) then
  for t:=0 to MAX_SHIP_SLOTS-1 do
  if ord(plship^.slot[t].hotkey)=48+i then
    ship_use_slot(plship,t);

if keypressed then
begin
c:=readkey;
case c of
  #27:begin
    game_menu_init;
    main_loop_proc:=@game_menu_loop;
    looptime:=0;
    end;
  #32:begin
  	game_paused:=not game_paused;
    looptime:=0;
    end;
  'h':show_hp:=not show_hp;
  'r':player_auto_repair:=not player_auto_repair;
  'q':player_repair(true);
  'i':show_item_panel:=not show_item_panel;
  #13:begin
    if player_lose then
      begin
      finish_current_level;
      loading_game;
      goto end_loop;
      end
    else
    if lvl_accomplished then
      begin
      finish_current_level;
      onstation_init(true,true);
      main_loop_proc:=@onstation_loop;
      goto end_loop;
      end;
    end;
  'c','C':show_statpanel:=not show_statpanel;
  #0:case readkey of
  	#59:begin
  		controls_init(main_loop_proc);
			main_loop_proc:=@controls_loop;
			looptime:=0;
			goto end_loop;
  		end;
    #67:begin
      load_game('saved_games\quicksave');
      goto end_loop;
      end;
    end;
  end;
end;
lt_input:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'creating screen image');
create_scr_img_gameprocess(scr_img);
lt_crimg:=getclocks-lt;lt:=getclocks;
putimage(0,0,scr_img);
lt_draw:=getclocks-lt;lt:=getclocks;

lvl_accomplished:=level_finish_check_proc();
if plship=nil then player_lose:=true;
time:=looptime*gamespeedkoef;
if MAINLOOP_REPORT then write(logfile,'Camera shaking...');
if cam_shake then update_camera_shaking(time/gamespeedkoef);
if MAINLOOP_REPORT then writeln(logfile,'ok');
if hp_sp>0 then hp_sp:=hp_sp+hp_sp_dir*time*300;
if (hp_sp<hp_sp_bl) then hp_sp_dir:=1;
if (hp_sp>100-hp_sp_bl) then hp_sp_dir:=-1;
if hp_sp<0 then hp_sp:=0;
if hp_sp>100 then hp_sp:=100;
if plship<>nil then
if round(plship^.life*100/ship_type[plship^.type_]^.life)<35 then hp_sp_bl:=35 else hp_sp_bl:=100;
switch_time:=(switch_time+round(time*1000))mod 6000;
lt_other:=lt_other+getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'updating bullets');
for i:=0 to MAX_BULLETS-1 do update_bullet(bullet[i],time);
lt_bullets:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'updating effects');
for i:=0 to MAX_EFFECTS-1 do update_effect(eff[i],time);
lt_eff:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'updating particles');
for i:=0 to MAX_PARTICLES-1 do update_particle(pt[i],time);
lt_part:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'updating asteroids');
for i:=0 to MAX_ASTEROIDS-1 do update_asteroid(ast[i],time);
lt_asteroids:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then writeln(logfile,'updating containers');
for i:=0 to MAX_CONTAINERS-1 do update_container(cont[i]);
lt_cont:=getclocks-lt;lt:=getclocks;
for i:=0 to MAX_EVENT_MESSAGES-1 do if ev_msg[i].exist then
  with ev_msg[i] do
  begin
  life:=life-time*1000;
  if life<=0 then
    begin
    exist:=false;
    text:='';
    end;
  end;
ast_cooldown:=ast_cooldown-round(time*1000);
if (random(100 div lvl_ast_prob)=0)and(ast_cooldown<=0) then
  begin
  create_random_asteroid;
  ast_cooldown:=random(lvl_ast_max_cooldown);
  end;
lt_other:=lt_other+getclocks-lt;lt:=getclocks;

if MAINLOOP_REPORT then write(logfile,'updating player ship...');
update_player_exp;
if (plship<>nil)and not game_paused then
with plship^ do
begin
if (mb=1)or(mb=5)or(keystate[VK_LEFT]=128)then
  begin
  if (tx>x)then tx:=x+1;//-getmaxx div 45;
  ship_com_left(plship,time);
  end;
if (mb=2)or(mb=6)or(keystate[VK_RIGHT]=128)then
  begin
  if (tx<x)then tx:=x+ship_w(plship)-1;//+getmaxx div 45;
  ship_com_right(plship,time);
  end;
if (mb=3)or(mb=5)or(mb=6)or(keystate[VK_DOWN]=128)then
  begin
  if (ty>y)then ty:=y+1;//-getmaxy div 45;
  ship_com_backward(plship,time);
  end;
if (mb=4)or(mb=5)or(mb=6)or(keystate[VK_UP]=128)then
  begin
  if (ty<y)then ty:=y+ship_h(plship)-1;//+getmaxy div 45;
  ship_com_forward(plship,time);
  end;
if tx<>x then scr_x:=round(scr_x+(tx-x)*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed);
if (y-scr_y>getmaxy div 3)and(ty-y>0)and(scr_y+getmaxy<lvl_screens*getmaxy) then scr_y:=scr_y+(ty-y)*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
if ty-ship_h(plship)<scr_y+5 then ty:=scr_y+ship_h(plship)+5;
if ty>lvl_screens*getmaxy then ty:=lvl_screens*getmaxy-5;
if player_auto_repair and(plship^.life/ship_type[plship^.type_]^.life<0.7) then player_repair(true);
end;//if
if MAINLOOP_REPORT then writeln(logfile,'ok');
lt_plship:=getclocks-lt;lt:=getclocks;
if MAINLOOP_REPORT then write(logfile,'updating ships...');
for i:=0 to MAX_SHIPS-1 do if ship[i]<>nil then update_ship(ship[i],time);
if MAINLOOP_REPORT then writeln(logfile,'ok');
lt_ships:=getclocks-lt;lt:=getclocks;
if (scr_x/lvl_bg[0].dist>getimagewidth(lvl_bg[0].img))or(scr_x/lvl_bg[0].dist<-getimagewidth(lvl_bg[0].img))then scr_x:=0;
end_loop:
if MAINLOOP_REPORT then writeln(logfile);
lt_other:=lt_other+getclocks-lt;lt:=getclocks;
end;//main loop
