procedure destroy_item_mod(var m:Pitem_modification);
begin
dispose(m);
m:=nil;
end;

procedure destroy_item(var itm:Pitem);
var i:integer;
begin
if itm<>nil then
  for i:=0 to MAX_ITEM_MODIFICATIONS-1 do destroy_item_mod(itm^.modif[i]);
dispose(itm);
itm:=nil;
end;

procedure add_item_modif(itm:PPitem;m0,param0:integer;val0:double);
var i:integer;
begin
i:=0;
while (i<MAX_ITEM_MODIFICATIONS-1)and(itm^^.modif[i]<>nil)do inc(i);
if (i<MAX_ITEM_MODIFICATIONS-1)and(itm^^.modif[i]=nil)then
  begin
  new(itm^^.modif[i]);
  itm^^.modif[i]^.m:=m0;
  itm^^.modif[i]^.param:=param0;
  itm^^.modif[i]^.val:=val0;
  end;
end;

function mweap_dmg1(blt:Pbullet_type;itm:PPitem):longint;
var i:integer;
  res:double;
begin
res:=blt^.dmg1;
if (itm<>nil)and(itm^<>nil)then
for i:=0 to MAX_ITEM_MODIFICATIONS-1 do
if (itm^^.modif[i]<>nil) then with itm^^.modif[i]^ do
if (param=IMW_DMG1)or(param=IMW_DMG)then
case m of
  IM_ADD:res:=res+val;
  IM_MUL:res:=res*val;
  end;
mweap_dmg1:=round(res);
end;
function mweap_dmg2(blt:Pbullet_type;itm:PPitem):longint;
var i:integer;
  res:double;
begin
res:=blt^.dmg2;
if (itm<>nil)and(itm^<>nil)then
for i:=0 to MAX_ITEM_MODIFICATIONS-1 do
if (itm^^.modif[i]<>nil) then with itm^^.modif[i]^ do
if (param=IMW_DMG2)or(param=IMW_DMG)then
case m of
  IM_ADD:res:=res+val;
  IM_MUL:res:=res*val;
  end;
mweap_dmg2:=round(res);
end;

procedure modify_fld_gen(itm:PPitem;exp:longint);
var sum,kol,param,m:longint;
  val:double;
begin
sum:=0;
kol:=0;
while (sum<exp)and(kol<MAX_ITEM_MODIFICATIONS) do
  begin
  param:=random(IMF_CLD);
  m:=random(IM_MAX);
  val:=0;
  case param of
    IMF_REFL:begin
      m:=IM_ADD;
      val:=random(10);
      if val>100-fld_gen[itm^^.indx]^.refl_chance then val:=100-fld_gen[itm^^.indx]^.refl_chance;
      end;
    end;
  if m=IM_ADD then sum:=sum+round(val*10);
  if m=IM_MUL then sum:=sum+round(val*100);
  add_item_modif(itm,m,param,val);
  inc(kol);
  end;
end;

procedure modify_weapon(itm:PPitem;exp:longint);
var sum,kol,m,param,i:longint;
  val:double;
begin
sum:=0;
kol:=0;
while (sum<exp)and(kol<MAX_ITEM_MODIFICATIONS) do
  begin
  param:=random(IMW_MAX);
  m:=random(IM_MAX);
  case m of
    IM_ADD:val:=random(round(sqrt(exp)/2));
    IM_MUL:val:=1+random(round(sqrt(sqrt(exp/2))/10*1000))/1000;
    end;
  if m=IM_ADD then sum:=sum+round(val*10);
  if m=IM_MUL then sum:=sum+round(val*100);
  add_item_modif(itm,m,param,val);
  inc(kol);
  end;
if mweap_dmg1(bullet_type[itm^^.indx],itm)>mweap_dmg2(bullet_type[itm^^.indx],itm)
then
 begin
 for i:=0 to MAX_ITEM_MODIFICATIONS-1 do
 if itm^^.modif[i]<>nil then
  with itm^^.modif[i]^ do
  begin
  if param=IMW_DMG1 then param:=IMW_DMG2 else
  if param=IMW_DMG2 then param:=IMW_DMG1;
  end;
 end;
end;

function new_item(type0,indx0:integer):PPitem;
var
  i,j:integer;
begin
i:=0;
while (i<MAX_ITEMS)and(item[i]<>nil)do inc(i);
if (i<MAX_ITEMS)and(item[i]=nil)then
  begin
  new(item[i]);
  item[i]^.type_:=type0;
  if indx0=-1 then
  case type0 of
    ITM_WEAPON:begin
      repeat indx0:=random(MAX_BULLET_TYPES-1)until bullet_type[indx0]<>nil;
      end;
    ITM_FLD_GEN:begin
      repeat indx0:=random(MAX_FLDGENERATORS-1)until fld_gen[indx0]<>nil;
      end;
    end;
  item[i]^.indx:=indx0;
  for j:=0 to MAX_ITEM_MODIFICATIONS-1 do item[i]^.modif[j]:=nil;
  new_item:=@item[i];
  end
else new_item:=nil;
end;

function calc_item_cost(itm:Pitem):longint;
var dmg:longint;
begin
if (itm<>nil)then
  case itm^.type_ of
    ITM_WEAPON:with bullet_type[itm^.indx]^ do
      begin
      dmg:=mweap_dmg1(bullet_type[itm^.indx],@itm)+mweap_dmg2(bullet_type[itm^.indx],@itm);
      calc_item_cost:=0;
      inc(calc_item_cost,round(power(vel,1.6)/200));
      inc(calc_item_cost,round(sqrt(cross)*power(dmg,1.85)/3));
      inc(calc_item_cost,round(sqr(1000/cooldown)/2));
      if lifetime<>-1 then inc(calc_item_cost,lifetime div 400)else calc_item_cost:=calc_item_cost*11 div 10;
      if guided then
        begin
        calc_item_cost:=calc_item_cost*3 div 2+round(power(dmg,1.5));
        inc(calc_item_cost,guide_dist+guide_time div 100);
        end;
//      calc_item_cost:=calc_item_cost div 2;
      end;
    ITM_FLD_GEN:with fld_gen[itm^.indx]^ do
      begin
      calc_item_cost:=round(sqr(refl_chance/2)*10);
      calc_item_cost:=calc_item_cost+round(sqr((100-koef*100)/(round(100-koef*100)div 10))*10);
      end;
    end
else calc_item_cost:=0;
end;
