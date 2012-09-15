function nearest_ship_dist(enemy:boolean;x,y:double;var res_ship:PPship):double;forward;
function new_effect(x0,y0,type0:longint;cycle0:boolean;layer0:integer):PPeff;
var i:integer;
begin
i:=0;
new_effect:=nil;
while (i<MAX_EFFECTS)and(eff[i]<>nil)do inc(i);
if (i<MAX_EFFECTS)and(eff[i]=nil)then
  begin
  if length(eff_type[type0]^.soundpath)>1 then add_sound(eff_type[type0]^.soundpath,false,true,x0,y0);
  new(eff[i]);
  new_effect:=@eff[i];
  with eff[i]^ do
    begin
    cycle:=cycle0;
    x:=x0;
    y:=y0;
    cur_fr:=0;
    time:=0;
    type_:=type0;
    layer:=layer0;
    end;
  end;
end;

procedure destroy_effect(var ef:Peff);
begin
dispose(ef);
ef:=nil;
end;

function eff_w(ef:Peff):integer;
begin
if ef<>nil then eff_w:=getimagewidth(eff_type[ef^.type_]^.img[ef^.cur_fr])
  else eff_w:=0;
end;
function eff_h(ef:Peff):integer;
begin
if ef<>nil then eff_h:=getimageheight(eff_type[ef^.type_]^.img[ef^.cur_fr])
  else eff_h:=0;
end;

procedure update_effect(var ef:Peff;dtime:double);
begin
if ef<>nil then
  if ef^.cur_fr>=eff_type[ef^.type_]^.kol_fr-1 then destroy_effect(ef)
  else
with ef^ do
  begin
  time:=time+dtime*1000;
  if time>eff_type[type_]^.frametime then
    begin
    inc(cur_fr);
    if (cur_fr>=eff_type[ef^.type_]^.kol_fr-1)and(cycle)then cur_fr:=0;
    time:=0;
    end;
  end;
end;

procedure new_effect_type(indx,frametime0:longint;mask0:boolean;path,maskpath,soundpath0:ansistring);
var i:longint;
  gif:PmediaGIF;
begin
new(eff_type[indx]);
with eff_type[indx]^ do
  begin
  soundpath:=soundpath0;
  frametime:=frametime0;
  mask:=mask0;
  new(gif,openmedia(path));
  gif^.SetRenderMode(gmrm_transparency);
  kol_fr:=gif^.getnumframe;
  kol_fr:=kol_fr mod (MAX_FRAMES+1);
  for i:=0 to kol_fr-1 do img[i]:=cloneimage(gif^.grabframe);
  dispose(gif,closemedia);
  if mask then
    begin
    new(gif,openmedia(maskpath));
    gif^.SetRenderMode(gmrm_transparency);
    kol_fr:=gif^.getnumframe;
    kol_fr:=kol_fr mod (MAX_FRAMES+1);
    for i:=0 to kol_fr-1 do img_mask[i]:=cloneimage(gif^.grabframe);
    dispose(gif,closemedia);
    end;
  end;
end;

procedure load_effect_type(path:ansistring);
var f:text;
    s:ansistring;
  indx0,frametime0,mask:longint;
  imgpath,maskpath,soundpath0:ansistring;
begin
soundpath0:='';
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  while not eof(f) do
    begin
    readln(f,s);
    if pos('indx',s)<>0 then val(getval(s,#39),indx0);
    if pos('mask',s)<>0 then val(getval(s,#39),mask);
    if pos('frtime',s)<>0 then val(getval(s,#39),frametime0);
    if pos('imgmain',s)<>0 then imgpath:=getval(s,#39);
    if pos('imgmask',s)<>0 then maskpath:=getval(s,#39);
    if pos('soundpath',s)<>0 then soundpath0:=getval(s,#39);
    end;
  close(f);
  new_effect_type(indx0,frametime0,mask=1,imgpath,maskpath,soundpath0);
  end;
end;

procedure new_ship_type(indx,eff0,turnkoef0:integer;scpr,ptype0,life0,att_cool0,plt_indx0:longint;imgpath0,imgmaskpath0:ansistring;
  turnab0:boolean;sk0:Tskill_list;slots0:Tship_slot_init);
var i:integer;
begin
new(ship_type[indx]);
with ship_type[indx]^ do
  begin
  att_cooldown:=att_cool0;
  score_prize:=scpr;
  eff_indx:=eff0;
  life:=life0;
  ptype:=ptype0;
  turnab:=turnab0;
  turnkoef:=turnkoef0/100;
  pilot_indx:=plt_indx0;
  for i:=0 to MAX_SHIP_SLOTS-1 do
    with slot0[i] do
      begin
      exist:=slots0[i].exist;
      dx:=slots0[i].dx;
      dy:=slots0[i].dy;
      itm_indx:=slots0[i].itm_indx;
      itm_type:=slots0[i].itm_type;
      hotkey:=slots0[i].hotkey;
      end;
  with sk do
    begin
    bullet_speed:=sk0.bullet_speed;
    move_speed:=sk0.move_speed;
    bullet_dmg:=sk0.bullet_dmg;
    attack_speed:=sk0.attack_speed;
    end;
  mainimgpath:=imgpath0;
  imgmaskpath:=imgmaskpath0;
  if turnab then
    begin
    for i:=1 to 11 do
      begin
      loadimagefile(itdetect,imgpath0,g[i-6].img,i);
      loadimagefile(itdetect,imgmaskpath0,g[i-6].img_mask,i);
      end;
    end
  else
    begin
    loadimagefile(itdetect,imgpath0,g[0].img,0);
    loadimagefile(itdetect,imgpath0,g[0].img_mask,2);
    end;
  end;
end;

procedure destroy_ship_type(var sht:Pship_type);
var i:integer;
begin
if sht<>nil then
  begin
  for i:=-5 to 5 do
    begin
    destroyimage(sht^.g[i].img);
    destroyimage(sht^.g[i].img_mask);
    end;
  end;
dispose(sht);
sht:=nil;
end;

procedure load_ship_type(var f:text);
var
    s,s2:ansistring;
  indx0,eff0,t,sc_prize0:longint;
  imgpath,imgmaskpath:ansistring;
  turnability,ptype0,life0,pilot_indx0,att_cooldown0,turnkoef0:integer;
  sk0:Tskill_list;
  slots:Tship_slot_init;
  c:char;
begin
for t:=0 to MAX_SHIP_SLOTS-1 do
with slots[t] do
  begin
  exist:=false;
  itm_indx:=-1;
  end;
turnability:=0;
readln(f,s);
  while (pos('[END]',s)=0)and not eof(f) do
    begin
    if pos('indx',s)<>0 then val(getval(s,#39),indx0);
    if pos('eff',s)<>0 then val(getval(s,#39),eff0);
    if pos('life',s)<>0 then val(getval(s,#39),life0);
    if pos('turnability',s)<>0 then val(getval(s,#39),turnability);
    if pos('turnkoef',s)<>0 then val(getval(s,#39),turnkoef0);
    if pos('ptype',s)<>0 then val(getval(s,#39),ptype0);
    if pos('scoreprize',s)<>0 then val(getval(s,#39),sc_prize0);
    if pos('mainimg',s)<>0 then imgpath:=getval(s,#39);
    if pos('imgmask',s)<>0 then imgmaskpath:=getval(s,#39);
    if pos('pilot',s)<>0 then val(getval(s,#39),pilot_indx0);
    if pos('att_cooldown',s)<>0 then val(getval(s,#39),att_cooldown0);
    if pos('addslot',s)<>0 then
      begin
      val(getval(s,#39),t);
      slots[t].exist:=true;
      val(getval(s,'"'),slots[t].dx);
      val(getval(s,'*'),slots[t].dy);
      val(getval(s,':'),slots[t].itm_type);
      val(getval(s,'^'),slots[t].itm_indx);
      slots[t].hotkey:=s[pos('|',s)+1];
      if not(slots[t].hotkey in ['0'..'9']) then slots[t].hotkey:=#0;
      end;
    if pos('skills',s)<>0 then
    with sk0 do
      begin
      bullet_speed:=100;
      move_speed:=100;
      bullet_dmg:=100;
      attack_speed:=100;
      while pos('{',s)=0 do readln(f,s);
      repeat
        begin
        readln(f,s);
        if pos('bullet_speed',s)<>0 then val(getval(s,#39),bullet_speed);
        if pos('move_speed',s)<>0 then val(getval(s,#39),move_speed);
        if pos('bullet_dmg',s)<>0 then val(getval(s,#39),bullet_dmg);
        if pos('attack_speed',s)<>0 then val(getval(s,#39),attack_speed);
        end until pos('}',s)<>0;
      bullet_speed:=bullet_speed/100;
      move_speed:=move_speed/100;
      bullet_dmg:=bullet_dmg/100;
      attack_speed:=attack_speed/100;
      end;//skills
    readln(f,s);
    end;//while
new_ship_type(indx0,eff0,turnkoef0,sc_prize0,ptype0,life0,att_cooldown0,pilot_indx0,imgpath,imgmaskpath,turnability=1,sk0,slots);
end;

procedure load_bullet_type(path:ansistring);
var f:text;
    s:ansistring;
  indx,vel0,eff0,cooldown0,ptype0,dmg1_,dmg2_,guided0,guide_dist0,
    guide_time0,lifetime0,cross0,alpha0,cr_destroy0,droplvl0,
    maxplife0,maxpoffs0,kolpt0,cam_shake0:longint;
  imgpath,weaponimgpath,name0,view_img_path0,soundpath0:ansistring;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  while not eof(f) do
    begin
    readln(f,s);
    if pos('indx',s)<>0 then val(getval(s,#39),indx);
    if pos('droplvl',s)<>0 then val(getval(s,#39),droplvl0);
    if pos('guide_dist',s)<>0 then val(getval(s,#39),guide_dist0);
    if pos('guide_time',s)<>0 then val(getval(s,#39),guide_time0);
    if pos('lifetime',s)<>0 then val(getval(s,#39),lifetime0);
    if pos('guided',s)<>0 then val(getval(s,#39),guided0);
    if pos('alpha',s)<>0 then val(getval(s,#39),alpha0);
    if pos('cross',s)<>0 then val(getval(s,#39),cross0);
    if pos('cr_destroy',s)<>0 then val(getval(s,#39),cr_destroy0);
    if pos('dmg1',s)<>0 then val(getval(s,#39),dmg1_);
    if pos('dmg2',s)<>0 then val(getval(s,#39),dmg2_);
    if pos('ptype',s)<>0 then val(getval(s,#39),ptype0);
    if pos('maxplife',s)<>0 then val(getval(s,#39),maxplife0);
    if pos('maxpoffs',s)<>0 then val(getval(s,#39),maxpoffs0);
    if pos('pkol',s)<>0 then val(getval(s,#39),kolpt0);
    if pos('vel',s)<>0 then val(getval(s,#39),vel0);
    if pos('eff',s)<>0 then val(getval(s,#39),eff0);
    if pos('cooldown',s)<>0 then val(getval(s,#39),cooldown0);
    if pos('mainimg',s)<>0 then imgpath:=getval(s,#39);
    if pos('weaponimg',s)<>0 then weaponimgpath:=getval(s,#39);
    if pos('viewimg',s)<>0 then view_img_path0:=getval(s,#39);
    if pos('camera_shaking',s)<>0 then val(getval(s,#39),cam_shake0);
    if pos('name',s)<>0 then name0:=getval(s,#39);
    if pos('soundpath',s)<>0 then soundpath0:=getval(s,#39);
    end;
  close(f);
  new(bullet_type[indx]);
  with bullet_type[indx]^ do
    begin
    iti.view_img_path:=view_img_path0;
    iti.name:=name0;
    iti.droplvl:=droplvl0;
    soundpath:=soundpath0;
    loadimagefile(itdetect,imgpath,img,0);
    loadimagefile(itdetect,weaponimgpath,iti.img,0);
    loadimagefile(itdetect,weaponimgpath,iti.img_mask,2);
    cooldown:=cooldown0;
    guided:=(guided0=1);
    if guided then
      begin
      guide_dist:=round((guide_dist0*getmaxx)/100);
      guide_time:=guide_time0;
      end;
    alpha:=alpha0;
    cross:=cross0;
    cr_destroy:=cr_destroy0=1;
    lifetime:=lifetime0;
    dmg1:=dmg1_;
    dmg2:=dmg2_;
    vel:=round((vel0*getmaxx)/100);
    eff_indx:=eff0;
    ptype:=ptype0;
    maxplife:=maxplife0;
    maxpoffs:=maxpoffs0/100;
    kolpt:=kolpt0;
    w:=getimagewidth(img);
    h:=getimageheight(img);
    camera_shake_koef:=cam_shake0/100;
    end;
  end;
end;

procedure load_field_generator_type(path:ansistring);
var f:text;
    s:ansistring;
  indx,cooldown0,droplvl0,koef0,reflect0,eff0:longint;
  name0,view_img_path0,item_img_path0:ansistring;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  while not eof(f) do
    begin
    readln(f,s);
    if pos('indx',s)<>0 then val(getval(s,#39),indx);
    if pos('koef',s)<>0 then val(getval(s,#39),koef0);
    if pos('eff',s)<>0 then val(getval(s,#39),eff0);
    if pos('reflect',s)<>0 then val(getval(s,#39),reflect0);
    if pos('droplvl',s)<>0 then val(getval(s,#39),droplvl0);
    if pos('cooldown',s)<>0 then val(getval(s,#39),cooldown0);
    if pos('viewimg',s)<>0 then view_img_path0:=getval(s,#39);
    if pos('itemimg',s)<>0 then item_img_path0:=getval(s,#39);
    if pos('name',s)<>0 then name0:=getval(s,#39);
    end;
  close(f);
  new(fld_gen[indx]);
  with fld_gen[indx]^ do
    begin
    iti.view_img_path:=view_img_path0;
    iti.name:=name0;
    iti.droplvl:=droplvl0;
    loadimagefile(itdetect,item_img_path0,iti.img,0);
    loadimagefile(itdetect,item_img_path0,iti.img_mask,2);
    cooldown:=cooldown0;
    koef:=koef0/100;
    refl_chance:=reflect0;
    eff_indx:=eff0;
    end;
  end;
end;

function ship_w(sh:Pship):longint;
begin
if sh<>nil then ship_w:=getimagewidth(ship_type[sh^.type_]^.g[sh^.coff].img);
end;
function ship_h(sh:Pship):longint;
begin
if sh<>nil then ship_h:=getimageheight(ship_type[sh^.type_]^.g[sh^.coff].img);
end;

function calc_exp_reward(sh:Pship):integer;
begin
if sh<>nil then
with ship_type[sh^.type_]^ do
  calc_exp_reward:=round(score_prize*(sk.move_speed+sk.bullet_speed))
  else calc_exp_reward:=0;
end;

procedure destroy_ship(var sh:Pship;cr_eff:boolean);
var i:integer;
begin
if cr_eff and (sh<>nil) then with sh^ do
  begin
  new_effect(round(x+ship_w(sh) div 2),round(y)-sign(dir)*ship_h(sh) div 2,ship_type[type_]^.eff_indx,false,6);
  if (rnd(lvl_cont_prob))or((boss_ship<>nil)and(sh=boss_ship^)) then
    create_random_container(x+ship_w(sh)div 2,y-sign(dir)*ship_h(sh)div 2,calc_exp_reward(sh));
  end;
if sh<>nil then
  begin
  for i:=0 to MAX_SHIP_SLOTS-1 do if sh^.slot[i].itm<>nil then destroy_item(sh^.slot[i].itm^);
  if sh^.fld_eff<>nil then destroy_effect(sh^.fld_eff^);
  end;
if (plship=sh)and(sh<>nil) then
  begin
  plship:=nil;
  remove_sound(main_sound);
  while main_sound=nil do main_sound:=add_sound('sfx\sw2.mp3',false,false,0,0);
  end;
dispose(sh);
sh:=nil;
end;

function ship_visible(sh:Pship;ex:integer):boolean;
var t:integer;
begin
if (sh<>nil)then
with sh^ do
  begin
  ship_visible:=((y>scr_y-ex)and(y<scr_y+getmaxy+ex))or((y-dir*ship_h(sh)>scr_y-ex)and(y-dir*ship_h(sh)<scr_y+getmaxy+ex));
  end else ship_visible:=false;
end;

function ship_new(x0,y0,type0,dir0:longint;enemy0:boolean;control0:byte;layer0:integer):PPship;
var i:longint;
begin
i:=0;
while (i<MAX_SHIPS)and(ship[i]<>nil)do inc(i);
if (i<MAX_SHIPS)and(ship[i]=nil)then
  begin
  new(ship[i]);
  ship_new:=@ship[i];
  with ship_new^^ do
    begin
    ai_action_cooldown:=0;
    att_cooldown:=0;
    fld:=0;
    bul_refl:=0;
    fld_eff:=nil;
    control:=control0;
    enemy:=enemy0;
    coff:=0;
    active:=false;
    type_:=type0;
    dir:=dir0;
    life:=ship_type[type_]^.life;
    x:=x0-ship_w(ship[i]) div 2;
    y:=y0+ship_h(ship[i]);
    tx:=x;ty:=y;
    plt:=pilot[ship_type[type_]^.pilot_indx];
    for i:=0 to MAX_SHIP_SLOTS-1 do slot_cooldown[i]:=0;
    layer:=layer0;
    for i:=0 to MAX_SHIP_SLOTS-1 do
    with ship_type[type_]^.slot0[i]do
      begin
      slot[i].itm_type:=itm_type;
      slot[i].exist:=exist;
      slot[i].dx:=dx;
      slot[i].dy:=dy;
      slot[i].hotkey:=hotkey;
      if itm_indx<>-1 then slot[i].itm:=new_item(itm_type,itm_indx) else slot[i].itm:=nil;
      end;
    end;
  ///
  end;
end;

procedure ship_com_left(ship:Pship;time:double);
begin
if (ship<>nil)and(abs(ship^.tx-ship^.x)<getmaxx div 2) then
  with ship^ do
  begin
  tx:=tx-SHIP_MOVE_CONST*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  if abs(tx-x)>5 then tx:=tx-time*50*abs(tx-x)/SHIP_MOVE_CONST;
  end;
end;
procedure ship_com_right(ship:Pship;time:double);
begin
if (ship<>nil)and(abs(ship^.tx-ship^.x)<getmaxx div 2) then
  with ship^ do
  begin
  tx:=tx+SHIP_MOVE_CONST*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  if abs(tx-x)>5 then tx:=tx+time*50*abs(tx-x)/SHIP_MOVE_CONST;
  end;
end;
procedure ship_com_forward(ship:Pship;time:double);
begin
if (ship<>nil)and(abs(ship^.ty-ship^.y)<getmaxy div 2) then
  with ship^ do
  begin
  ty:=ty+SHIP_MOVE_CONST*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  if abs(ty-y)>5 then ty:=ty+time*50*abs(ty-y)/SHIP_MOVE_CONST;
  end;
end;
procedure ship_com_backward(ship:Pship;time:double);
begin
if (ship<>nil)and(abs(ship^.ty-ship^.y)<getmaxy div 2) then
  with ship^ do
  begin
  ty:=ty-SHIP_MOVE_CONST*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  if abs(ty-y)>5 then ty:=ty-time*50*abs(ty-y)/SHIP_MOVE_CONST;
  end;
end;

procedure ship_attack(sh:Pship;slot_indx:integer);
var slots:array[0..MAX_SHIP_SLOTS-1]of integer;
  kol,i:integer;
begin
if (sh<>nil)and(sh^.att_cooldown<=0) then
with sh^ do
  begin
  kol:=1;
  if slot_indx=-1 then
    begin
    kol:=0;
    for i:=0 to MAX_SHIP_SLOTS-1 do
    if (sh^.slot_cooldown[i]<=0)and
      (sh^.slot[i].exist)and
      (sh^.slot[i].itm<>nil)and
      (sh^.slot[i].itm^<>nil)and
      (sh^.slot[i].itm^^.type_=ITM_WEAPON)then
        begin
        slots[kol]:=i;
        inc(kol);
        end;
    slot_indx:=slots[random(kol)];
    end;
  if kol<>0 then
  if (sh^.slot_cooldown[slot_indx]<=0)and
    (sh^.slot[slot_indx].exist)and
    (sh^.slot[slot_indx].itm<>nil)and
    (sh^.slot[slot_indx].itm^<>nil)and
    (sh^.slot[slot_indx].itm^^.type_=ITM_WEAPON)then
    begin
    if sh=plship then add_cam_shake(0.1,vec_new(0,1));
    new_bullet(x+ship_w(sh)div 2+sh^.slot[slot_indx].dx,
      y-sign(dir)*ship_h(sh)div 2-sh^.slot[slot_indx].dy,
      sh^.slot[slot_indx].itm^^.indx,dir,enemy,
      ship_type[type_]^.sk.bullet_speed*plt^.sk.bullet_speed,
      ship_type[type_]^.sk.bullet_dmg*plt^.sk.bullet_dmg,2,sh^.slot[slot_indx].itm);
    slot_cooldown[slot_indx]:=bullet_type[sh^.slot[slot_indx].itm^^.indx]^.cooldown/ship_type[type_]^.sk.attack_speed;
    att_cooldown:=att_cooldown+ship_type[type_]^.att_cooldown/ship_type[type_]^.sk.attack_speed;
    end;
  end;
end;

procedure ship_use_fld_gen(ship:Pship;indx:integer);
var
	i:integer;
label end_;
begin
if ship=nil then goto end_;
i:=0;
while (indx=-1)and(i<MAX_SHIP_SLOTS) do
	begin
  if (ship^.slot[i].itm_type=ITM_FLD_GEN)and(ship^.slot[i].itm<>nil)and(fld_gen[ship^.slot[i].itm^^.indx]<>nil) then indx:=i;
  inc(i);
  end;
if indx=-1 then goto end_;
if (fld_gen[ship^.slot[indx].itm^^.indx]<>nil)and(ship^.slot_cooldown[indx]<=0) then
  ship^.slot_cooldown[indx]:=fld_gen[ship^.slot[indx].itm^^.indx]^.cooldown else goto end_;
ship^.fld:=1;
ship^.bul_refl:=fld_gen[ship^.slot[indx].itm^^.indx]^.refl_chance;
ship^.fld_force:=fld_gen[ship^.slot[indx].itm^^.indx]^.koef;
if ship^.fld_eff<>nil then destroy_effect(ship^.fld_eff^);
ship^.fld_eff:=new_effect(round(ship^.x+ship_w(ship)div 2),round(ship^.y-ship^.dir*ship_h(ship) div 2),fld_gen[ship^.slot[indx].itm^^.indx]^.eff_indx,true,ship^.layer+1);
end_:
end;

function slot_by_hotkey(sh:Pship;c:char):integer;
label end_;
begin
if sh=nil then goto end_;
slot_by_hotkey:=-1;
for i:=0 to MAX_SHIP_SLOTS-1 do
if sh^.slot[i].exist and
(c=sh^.slot[i].hotkey)and
(sh^.slot[i].itm<>nil)and
((sh^.slot[i].itm^^.type_=ITM_WEAPON)or(sh^.slot[i].itm^^.type_=ITM_FLD_GEN))and
(sh^.slot[i].itm^^.indx<>-1) then slot_by_hotkey:=i;
end_:
end;

procedure ship_use_slot(ship:Pship;indx:integer);
label end_;
begin
if ship=nil then goto end_;
if indx=-1 then indx:=random(MAX_SHIP_SLOTS);
if (ship^.slot[indx].exist)and(ship^.slot_cooldown[indx]<=0)and
  (ship^.slot[indx].itm<>nil)then
  case ship^.slot[indx].itm^^.type_ of
    ITM_WEAPON:ship_attack(ship,indx);
    ITM_FLD_GEN:ship_use_fld_gen(ship,indx);
    end;
end_:
end;

procedure update_ship_AI(sh:Pship;time:double);
var t:longint;
  target:PPship;
label end_;
begin
if sh=nil then goto end_;
dec(sh^.ai_action_cooldown,round(time*1000));
if sh^.ai_action_cooldown<0 then sh^.ai_action_cooldown:=0;
if plship<>nil then
if sh^.enemy then
  with sh^ do
  begin
  if (abs(plship^.x-x)>getmaxx div 3)and(ai_action_cooldown<1000) then
    begin
    case sign(plship^.tx-x)of
      -1:ship_com_left(sh,time);
       1:ship_com_right(sh,time);
      end;
    end;
  if ai_action_cooldown<=0 then
  	begin
  	ty:=plship^.y+getmaxy div 8+random(getmaxy*6 div 8);
    tx:=plship^.x+getmaxx div 4-random(getmaxx*2 div 4);
    ai_action_cooldown:=random(1500);
    end;
  ship_attack(sh,-1);
  if ty>lvl_screens*getmaxy then ty:=lvl_screens*getmaxy-5-ship_h(sh);
  end
else
  with sh^ do
  begin
  if (sh<>plship)and(abs(plship^.ty-ty)>getmaxy div 6)or(random(100)=0)or(y<scr_y)then
    ty:=plship^.y+getmaxy*2 div 8-random(getmaxy div 8);
  if (nearest_ship_dist(not enemy,x,y,target)<getmaxy)and(target<>nil) then
    begin
    ship_attack(sh,-1);
    if (abs(tx-target^^.tx)>getmaxx div 5)or(random(100)=0)then
      tx:=target^^.tx+getmaxx div 8-random(getmaxx*2 div 8);
    end
  else
    if abs(plship^.x-tx)<ship_w(plship)+ship_w(sh)+10 then
      case -sign(plship^.x-tx)of
      -1:ship_com_left(sh,time);
       1:ship_com_right(sh,time);
      end;
  end;
ship_use_fld_gen(sh,-1);
end_:
end;

procedure ship_take_damage(sh:Pship;dmg:double);
label end_;
begin
if sh=nil then goto end_ else
with sh^ do
  begin
  if fld>0 then dmg:=dmg*fld_force;
  dec(life,round(dmg));
  if fld>0 then
    begin
    dmg:=(dmg/fld_force)*(1-fld_force)/ship_type[type_]^.life;
    fld:=fld-dmg;
    if (fld<=0)and(fld_eff<>nil) then
      begin
      destroy_effect(sh^.fld_eff^);
      fld_eff:=nil;
      end;
    end;
  end;
end_:
end;

procedure update_ship(var ship:Pship;time:double);
var kill:boolean;
  i,t,d1,d2,kol:longint;
  doub,dy:double;
begin
kill:=false;
with ship^ do
  begin
  if not active then active:=scr_y+getmaxy*3 div 2>y;
  if active and(control=CONTROL_AUTO)and not game_paused and ship_visible(ship,getmaxy div 2) then update_ship_AI(ship,time);
  if (plship=nil)and(ship^.enemy) then
    begin
    ty:=scr_y-getmaxy*2;
    tx:=tx+getmaxx div 20-random(getmaxx*2 div 20);
    end;
  end;

with ship^ do
  begin
  for i:=0 to MAX_SHIP_SLOTS-1 do
    begin
    if slot_cooldown[i]>0 then slot_cooldown[i]:=slot_cooldown[i]-time*1000 else slot_cooldown[i]:=0;
    end;
  if att_cooldown>0 then att_cooldown:=att_cooldown-time*1000 else att_cooldown:=0;
  if x<>tx then x:=x+(tx-x)*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  if y<>ty then
  	begin
  	dy:=(ty-y)*time*ship_type[type_]^.sk.move_speed*plt^.sk.move_speed;
  	y:=y+dy;
  	end else dy:=0;
  if x<0 then
    begin x:=getmaxx-ship_w(ship);tx:=tx+getmaxx-1;end;
  if x>=getmaxx then
    begin x:=1;tx:=tx-getmaxx;end;
  if ship_type[type_]^.turnab then
    begin
    t:=round(ship_type[type_]^.turnkoef*(tx-x)/ship_w(ship));
    if t<coff then dec(coff);
    if t>coff then inc(coff);
    if coff>5 then coff:=5;
    if coff<-5 then coff:=-5;
    end;
  if dir=-1 then t:=ship_h(ship) else t:=0;
  if ship_visible(ship,getmaxy div 2)then
  for i:=0 to MAX_BULLETS-1 do
    if (bullet[i]<>nil)and
    (bullet[i]^.enemy<>enemy)and(bullet[i]^.prev_target<>@ship)and
    (((bullet[i]^.x<=x+ship_w(ship))and(bullet[i]^.y>=y+t-ship_h(ship))and(bullet[i]^.x>=x)and(bullet[i]^.y<=y+t))or
      lines_crossed(bullet[i]^.x,bullet[i]^.y,bullet[i]^.px,bullet[i]^.py,x,y,x+ship_w(ship),y-dir*ship_h(ship),nil,nil)or
      lines_crossed(bullet[i]^.x,bullet[i]^.y,bullet[i]^.px,bullet[i]^.py,x,y-dir*ship_h(ship),x+ship_w(ship),y,nil,nil))
      then
      with bullet_type[bullet[i]^.type_]^ do
      begin
      if bullet[i]^.cross>0 then
        begin
        if ship=plship then hp_sp:=100;
        hp_sp_dir:=-1;
        d1:=mweap_dmg1(bullet_type[bullet[i]^.type_],bullet[i]^.par);
        d2:=mweap_dmg2(bullet_type[bullet[i]^.type_],bullet[i]^.par);
        doub:=(d1+random(abs(d2-d1)))*bullet[i]^.dmg_koef;
        if (bullet_type[bullet[i]^.type_]^.guided)and(fld>0)and(random(100 div bul_refl)=0) then
          begin
          bullet[i]^.guide_time:=bullet_type[bullet[i]^.type_]^.guide_time;
          bullet[i]^.enemy:=not bullet[i]^.enemy;
          bullet[i]^.lifetime:=bullet_type[bullet[i]^.type_]^.lifetime;
          end
        else
          begin
          ship_take_damage(ship,doub);
          dec(bullet[i]^.cross);
          if plship<>nil then
            begin
            doub:=1-sqrt(sqr(plship^.x-bullet[i]^.x)+sqr(plship^.y-bullet[i]^.y))/(getmaxy*SILENCE_DIST);
            doub:=doub*camera_shake_koef;
            add_cam_shake(doub,vec_rand);
            end;
          end;
        bullet[i]^.prev_target:=@ship;
        bullet[i]^.target:=nil;
        end;
      if cr_destroy and(bullet[i]^.cross<=0) then destroy_bullet(bullet[i],true);
      end;
  if life<=0 then
    begin
    kill:=true;
    if enemy then with ship_type[type_]^ do
      begin
      lvl_exp_collected:=lvl_exp_collected+round(score_prize*(sk.move_speed+sk.bullet_speed));
      exp:=exp+calc_exp_reward(ship);
      end;
    end;
  if fld<0 then fld:=0;
  t:=-dir*ship_h(ship);
  if(fld_eff<>nil)then
  with fld_eff^^ do
    begin
    x:=round(ship^.x+ship_w(ship)div 2);
    y:=round(ship^.y+t div 2);
    end;
  if ship_type[type_]^.ptype<>-1 then
  	d1:=getimagewidth(ptt[ship_type[type_]^.ptype]^.img)div 5;
  if not game_paused and(ship_type[type_]^.ptype<>-1)and(abs(ty-y)>getmaxy div 20) then
  	begin
		kol:=round(dy/2);
    if kol=0 then kol:=1;
  	for i:=1 to kol do
    	new_particle(x+ship_w(ship)div 2-d1+random(d1*2),y+t-sign(dy)*random(round(abs(dy)*1000))div 1000,random(500),0,-dir*150,ship_type[type_]^.ptype,3);
    end;
  end;
if ship^.y<scr_y-getmaxy div 2 then destroy_ship(ship,false) else
if kill then destroy_ship(ship,true);
end;

procedure new_bullet(x0,y0:double;type0,dir:integer;enemy0:boolean;vel_koef,dmg_koef0:double;layer0:integer;par0:PPitem);
begin
i:=0;
while (i<MAX_BULLETS)and(bullet[i]<>nil)do inc(i);
if (i<MAX_BULLETS)and(bullet[i]=nil) then
  begin
  if length(bullet_type[type0]^.soundpath)>1 then add_sound(bullet_type[type0]^.soundpath,false,true,x0,y0);
  new(bullet[i]);
  with bullet[i]^ do
    begin
    type_:=type0;
    target:=nil;
    prev_target:=nil;
    cross:=bullet_type[type_]^.cross;
    guide_time:=bullet_type[type_]^.guide_time;
    lifetime:=bullet_type[type_]^.lifetime;
    enemy:=enemy0;
    x:=x0;y:=y0;
    px:=x0;py:=y0;
    velx:=0;
    vely:=dir*bullet_type[type_]^.vel*vel_koef;
    dmg_koef:=dmg_koef0;
    layer:=layer0;
    par:=par0;
    end;
  end;
end;

procedure destroy_bullet(var b:Pbullet;cr_eff:boolean);
begin
if b<>nil then
with b^ do
  begin
  if (cr_eff)and(bullet_type[type_]^.eff_indx<>-1) then
    new_effect(round(x),round(y),bullet_type[type_]^.eff_indx,false,6);
  end;
if b<>nil then
  begin
  dispose(b);
  b:=nil;
  end;
end;

function nearest_ship_dist(enemy:boolean;x,y:double;var res_ship:PPship):double;
var res:PPship;
  i,indx:integer;
  dist,tdist:double;
begin
res:=nil;
res_ship:=nil;
dist:=-1;
i:=0;
while (i<MAX_SHIPS-1) do
  begin
  if (ship[i]<>nil)and(ship[i]^.enemy=enemy)then
    begin
    tdist:=sqrt(sqr(x-ship[i]^.x)+sqr(y-ship[i]^.y));
    if (tdist<dist)or(dist=-1) then begin dist:=tdist;indx:=i;end;
    end;
  inc(i);
  end;
if dist<>-1 then res_ship:=@ship[indx];
nearest_ship_dist:=dist;
end;

function bullet_find_target(b:Pbullet):PPship;
var tship:PPship;
begin
bullet_find_target:=nil;
if b<>nil then
with b^ do
if (nearest_ship_dist(not enemy,x,y,tship)<=bullet_type[type_]^.guide_dist)and
  (prev_target<>tship) then bullet_find_target:=tship;
end;

procedure update_bullet(var b:Pbullet;time:double);
var i,kol:integer;
  dx,dy,k:double;
  vel_vec,dir_vec:Tvec;
label end_;
begin
if b=nil then goto end_;
with b^ do
  begin
  if (target<>nil)and(target^=nil)then target:=nil;
  if (bullet_type[type_]^.guided)and(guide_time>0)and(cross>0)then
    begin
    guide_time:=guide_time-time*1000;
    if target=nil then target:=bullet_find_target(b);

    if (target<>nil)and(target^<>nil)then
      begin
      dir_vec:=vec_new(target^^.x+ship_w(target^)div 2-x,target^^.y-sign(target^^.dir)*ship_h(target^)div 2-y);
      vel_vec:=vec_new(velx,vely);
      with vec_setangrel(vel_vec,vec_angle(dir_vec)-vec_angle(vel_vec))do begin velx:=x;vely:=y;end;
      end;
    end;
  //
  if lifetime<>-1 then
    begin
    lifetime:=lifetime-time*1000;
    if lifetime<0 then lifetime:=0;
    end;
  px:=x;py:=y;
  x:=x+velx*time;
  y:=y+vely*time;
  if velx=0 then
    begin
    dy:=-sign(vely)*bullet_type[type_]^.h;
    dx:=0;
    end
  else
    begin
    k:=arccos(velx/sqrt(sqr(velx)+sqr(vely)))*180/pi;
    k:=k+360;
    if k>360 then k:=k-360;
    dx:=-sign(velx)*abs(bullet_type[type_]^.h*(3/2)*cos(k*pi/180));
    dy:=-sign(vely)*bullet_type[type_]^.h*(3/2)*sin(k*pi/180)+sign(vely)*bullet_type[type_]^.h/2;
    end;
  if (bullet_type[type_]^.ptype<>-1)and not game_paused then
  	begin
		kol:=round(sqrt(sqr(vely)+sqr(velx))*time/2/bullet_type[type_]^.kolpt);
    if kol=0 then kol:=1;
    for i:=1 to kol do
    new_particle(x+dx-sign(velx)*round((abs(velx)*time)/2)-round(bullet_type[type_]^.w*bullet_type[type_]^.maxpoffs/2)
      +sign(velx)*random(round((abs(velx)*time)))+random(round(bullet_type[type_]^.w*bullet_type[type_]^.maxpoffs)),
    	y+dy-sign(vely)*round(abs(vely)*time)+sign(vely)*random(round(abs(vely)*time)),
    	random(bullet_type[type_]^.maxplife),0,0,bullet_type[type_]^.ptype,3);
    end;
  end;
if (b^.y>getmaxy*3 div 2+scr_y+bullet_type[b^.type_]^.h)
  or(b^.y<scr_y-getmaxy div 4)or(b^.x<0)or(b^.x>getmaxx)then destroy_bullet(b,false)
else if(b^.lifetime=0) then destroy_bullet(b,true);
end_:
end;

procedure new_particle(x0,y0,life0,velx0,vely0:double;type0:integer;layer0:integer);
begin
i:=0;
while (i<MAX_PARTICLES)and(pt[i]<>nil)do inc(i);
if (life0>0)and(i<MAX_PARTICLES)and(pt[i]=nil) then
  begin
  new(pt[i]);
  with pt[i]^ do
    begin
    alpha:=0;
    type_:=type0;
    life:=life0;
    maxlife:=life0;
    x:=x0;
    y:=y0;
    velx:=velx0;
    vely:=vely0;
    layer:=layer0;
    end;
  end;
end;

procedure destroy_particle(var p:Pparticle);
begin
dispose(p);
p:=nil;
end;

procedure update_particle(var p:Pparticle;time:double);
label end_;
begin
if p=nil then goto end_;
with p^ do
  begin
  life:=life-time*1000;
  alpha:=255*p^.life/p^.maxlife-5;
  x:=x+velx*time;
  y:=y+vely*time;
  if x>getmaxx then x:=0;
  if x<0 then x:=getmaxx;
  end;
if (p^.life<=0)or(p^.alpha<=128) then destroy_particle(p);
end_:
end;

procedure new_particle_type(indx:integer;imgpath:ansistring);
var i:integer;
begin
new(ptt[indx]);
with ptt[indx]^ do
  begin
  loadimagefile(itdetect,imgpath,img,0);
  loadimagefile(itdetect,imgpath,img_mask,2);
  w:=getimagewidth(img);
  h:=getimageheight(img);
  end;
end;

procedure load_particle_type(path:ansistring);
var f:text;
    s:ansistring;
  indx0,vel0,effindx0,cooldown0:longint;
  imgpath:ansistring;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  while not eof(f) do
    begin
    readln(f,s);
    if pos('indx',s)<>0 then val(getval(s,#39),indx0);
    if pos('img',s)<>0 then imgpath:=getval(s,#39);
    end;
  close(f);
  new_particle_type(indx0,imgpath);
  end;
end;

procedure destroy_pilot(var plt:Ppilot);
begin
dispose(plt);
plt:=nil;
end;

procedure load_pilot_type(var f:text);
var
  s,name0:ansistring;
  sk0:Tpilot_skill_list;
  indx:longint;
begin
readln(f,s);
  while pos('[END]',s)=0 do
    begin
    if pos('indx',s)<>0 then val(getval(s,#39),indx);
    if pos('name',s)<>0 then name0:=getval(s,#39);
    if pos('skills',s)<>0 then
    with sk0 do
      begin
      bullet_speed:=100;
      move_speed:=100;
      bullet_dmg:=100;
      while pos('{',s)=0 do readln(f,s);
      repeat
        begin
        readln(f,s);
        if pos('bullet_speed',s)<>0 then val(getval(s,#39),bullet_speed);
        if pos('move_speed',s)<>0 then val(getval(s,#39),move_speed);
        if pos('bullet_dmg',s)<>0 then val(getval(s,#39),bullet_dmg);
        if pos('merchant',s)<>0 then val(getval(s,#39),merchant);
        end until pos('}',s)<>0;
      bullet_speed:=bullet_speed/100;
      move_speed:=move_speed/100;
      bullet_dmg:=bullet_dmg/100;
      merchant:=merchant/100;
      end;//skills
    readln(f,s);
    end;
  new(pilot[indx]);
  with pilot[indx]^.sk do
    begin
    bullet_speed:=sk0.bullet_speed;
    move_speed:=sk0.move_speed;
    bullet_dmg:=sk0.bullet_dmg;
    merchant:=sk0.merchant;
    end;
  pilot[indx]^.name:=name0;
end;

procedure load_asteroid_type(path:ansistring);
var f:text;
    s:ansistring;
    i,indx,eff_indx0,deatheff_indx0,dmg_perc0,childmin0,childmax0,
      cam_shake0,kol_child_types0:longint;
    child0:array[0..MAX_ASTEROID_CHILD_TYPES-1]of integer;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  for i:=0 to MAX_ASTEROID_CHILD_TYPES-1 do child0[i]:=-1;
  kol_child_types0:=0;
  while not eof(f) do
    begin
    readln(f,s);
    if pos('indx',s)<>0 then val(getval(s,#39),indx);
    if pos('maineff',s)<>0 then val(getval(s,#39),eff_indx0);
    if pos('deatheff',s)<>0 then val(getval(s,#39),deatheff_indx0);
    if pos('dmg_perc',s)<>0 then val(getval(s,#39),dmg_perc0);
    if pos('camera_shaking',s)<>0 then val(getval(s,#39),cam_shake0);
    if pos('minkolchild',s)<>0 then val(getval(s,#39),childmin0);
    if pos('maxkolchild',s)<>0 then val(getval(s,#39),childmax0);
    if pos('addchild',s)<>0 then
      begin
      val(getval(s,#39),child0[kol_child_types0]);
      inc(kol_child_types0);
      end;
    end;
  close(f);
  new(ast_type[indx]);
  with ast_type[indx]^ do
    begin
    eff_indx:=eff_indx0;
    deatheff_indx:=deatheff_indx0;
    dmg_perc:=dmg_perc0;
    w:=getimagewidth(eff_type[eff_indx]^.img[0]);
    h:=getimageheight(eff_type[eff_indx]^.img[0]);
    camera_shake_koef:=cam_shake0/100;
    layer:=3;
    childmin:=childmin0;
    childmax:=childmax0;
    child_kol_types:=kol_child_types0;
    for i:=0 to MAX_ASTEROID_CHILD_TYPES-1 do child[i]:=child0[i];
    end;
  end;
end;

procedure destroy_asteroid(var ast:Pasteroid);
begin
if (ast<>nil)and(ast^.eff<>nil)and(ast^.eff^<>nil)then destroy_effect(ast^.eff^);
dispose(ast);
ast:=nil;
end;

function asteroid_new(type0:integer):PPasteroid;
var i:integer;
begin
i:=0;
asteroid_new:=nil;
while (i<MAX_ASTEROIDS)and(ast[i]<>nil)do inc(i);
if (i<MAX_ASTEROIDS)and(ast[i]=nil)then
  begin
  new(ast[i]);
  asteroid_new:=@ast[i];
  if ast[i]<>nil then
    begin
    ast[i]^.type_:=type0;
    ast[i]^.eff:=new_effect(-getmaxx,0,ast_type[type0]^.eff_indx,true,ast_type[type0]^.layer);
    if ast[i]^.eff<>nil then
    	ast[i]^.eff^^.cur_fr:=random(eff_type[ast_type[type0]^.eff_indx]^.kol_fr);
    end;
  if ast[i]^.eff=nil then
  	begin
  	destroy_asteroid(ast[i]);
  	asteroid_new:=nil;
  	end;
  end;
end;

procedure create_asteroid_children(ast:Pasteroid;x0,y0:double);
var kol,i,type0:integer;
  nast:PPasteroid;
  nx,ny:double;
label end_;
begin
if ast=nil then goto end_;
with ast_type[ast^.type_]^ do kol:=childmin+random(childmax-childmin+1);
for i:=1 to kol do
  begin
  type0:=ast_type[ast^.type_]^.child[random(ast_type[ast^.type_]^.child_kol_types)];
  nast:=asteroid_new(type0);
  if (nast<>nil)and(nast^<>nil) then
  with nast^^ do
    begin
    x:=ast^.x;y:=ast^.y;
    accx:=0;accy:=0;
    nx:=x-ast_type[type_]^.w div 2+random(round((ast_type[type_]^.w)*1000))/1000;
    ny:=y+ast_type[type_]^.h div 2-random(round((ast_type[type_]^.h)*1000))/1000;
    with vec_setang(vec_new(ast^.velx/kol/2,ast^.vely/kol/2),vec_angle(vec_new(nx-x0,ny-y0)))do
      begin
      velx:=x;
      vely:=y;
      end;
    end;
  end;
end_:
end;

procedure update_asteroid(var ast:Pasteroid;time:double);
var kill:boolean;
  i:integer;
  doub:double;
begin
if ast<>nil then
begin
kill:=false;
with ast^ do
  begin
  velx:=velx+accx*time;
  vely:=vely+accy*time;
  x:=x+velx*time;
  y:=y+vely*time;
  kill:=(y<scr_y-getmaxy)or(y>scr_y+getmaxy*2)or
    ((velx>0)and(x>getmaxx*3 div 2))or((velx<0)and(x<-getmaxx div 2))or(type_<0);
  if (eff<>nil)and(eff^<>nil) then
    begin
    eff^^.x:=round(x);
    eff^^.y:=round(y);
    for i:=0 to MAX_SHIPS-1 do
    if (ship[i]<>nil)and(ship_visible(ship[i],getmaxy))and(((boss_ship<>nil)and(ship[i]<>boss_ship^))or(lvl_bosstype=-1)) then
    begin
    if not(kill)and rect_cross(round(ship[i]^.x),round(ship[i]^.y),round(ship[i]^.x)+ship_w(ship[i]),round(ship[i]^.y)-ship[i]^.dir*ship_h(ship[i]),
      round(x)-eff_w(eff^)div 2,round(y)+eff_h(eff^)div 2,round(x)+eff_w(eff^)div 2,round(y)-eff_h(eff^)div 2) then
        begin
        kill:=true;
        add_sound('sfx\explosions\expl3.wav',false,true,ship[i]^.x,ship[i]^.y);
        ship_take_damage(ship[i],round(ship_type[ship[i]^.type_]^.life*ast_type[type_]^.dmg_perc/100));
        if (ship[i]=plship)and(ship[i]^.life<=0) then ship[i]^.life:=1;
        if plship<>nil then
          begin
          doub:=1-sqrt(sqr(plship^.x-x)+sqr(plship^.y-y))/(getmaxy*SILENCE_DIST);
          doub:=doub*ast_type[type_]^.camera_shake_koef;
          add_cam_shake(doub,vec_rand);
          end;
        new_effect(round(x),round(y),ast_type[type_]^.deatheff_indx,false,ast_type[type_]^.layer);
        if ast_type[type_]^.child_kol_types>0 then create_asteroid_children(ast,ship[i]^.x,ship[i]^.y);
        end;
    end;
    if not kill then
    	for i:=1 to MAX_BULLETS-1 do
      if (not kill)and(bullet[i]<>nil)and(
      	point_in_rect(round(bullet[i]^.x),round(bullet[i]^.y),eff^^.x-eff_w(eff^)div 2,eff^^.y-eff_h(eff^)div 2,eff^^.x+eff_w(eff^)div 2,eff^^.y+eff_h(eff^)div 2)or
        (lines_crossed(bullet[i]^.x,bullet[i]^.y,bullet[i]^.px,bullet[i]^.py,eff^^.x-eff_w(eff^)div 2,eff^^.y-eff_h(eff^)div 2,eff^^.x+eff_w(eff^)div 2,eff^^.y+eff_h(eff^)div 2,nil,nil)or
        lines_crossed(bullet[i]^.x,bullet[i]^.y,bullet[i]^.px,bullet[i]^.py,eff^^.x-eff_w(eff^)div 2,eff^^.y+eff_h(eff^)div 2,eff^^.x+eff_w(eff^)div 2,eff^^.y-eff_h(eff^)div 2,nil,nil)))then
      	begin
        kill:=true;
        add_sound('sfx\explosions\expl3.wav',false,true,ship[i]^.x,ship[i]^.y);
        new_effect(round(x),round(y),ast_type[type_]^.deatheff_indx,false,ast_type[type_]^.layer);
//        if ast_type[type_]^.child_kol_types>0 then create_asteroid_children(ast,ship[i]^.x,ship[i]^.y);
        dec(bullet[i]^.cross);
        if bullet_type[bullet[i]^.type_]^.cr_destroy and(bullet[i]^.cross<=0) then destroy_bullet(bullet[i],true);
        end;
    end
  else
  	begin
  	kill:=true;
  	end;
  end;
if kill then destroy_asteroid(ast);
end;
end;

procedure create_random_asteroid;
var i,type0:integer;
  nast:PPasteroid;
begin
type0:=lvl_ast_types[random(lvl_kol_ast_types)];
nast:=asteroid_new(type0);
if (nast<>nil)and(nast^<>nil) then
with nast^^ do
  begin
  velx:=getmaxy*lvl_ast_minvelx-round(getmaxy*lvl_ast_velxoffs)div 2+random(round(getmaxy*lvl_ast_velxoffs));
  vely:=getmaxy*lvl_ast_minvely-round(getmaxy*lvl_ast_velyoffs)div 2+random(round(getmaxy*lvl_ast_velyoffs));
  accx:=0;
  accy:=random(getmaxy*3 div 4)-getmaxy*3 div 8;
  case random(2)of
    0:begin x:=-getmaxx div 4;end;
    1:begin x:=getmaxx+getmaxx div 4;velx:=-velx;end;
    end;
  y:=scr_y+getmaxy div 4+random(getmaxy*3 div 4);
  end;
end;
