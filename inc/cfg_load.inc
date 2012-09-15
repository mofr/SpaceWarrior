procedure load_all_files(path:ansistring);
var f,f2:text;
  s:ansistring;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln('ERROR: file not found:"',path,'"') else
  begin
  while not eof(f) do
    begin
    readln(f,s);
    if s='[SHIPS]'then
      begin
      if mainloop_report then write('loading ship types...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        assign(f2,s);
        {$i-}reset(f2);{$i+}
        if ioresult<>0 then writeln('ERROR: file not found:"',s,'"') else
          begin
          load_ship_type(f2);
          close(f2);
          end;
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//ships
    if s='[EFFECTS]'then
      begin
      if mainloop_report then write('loading effects...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_effect_type(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//effects
    if s='[BULLETS]'then
      begin
      if mainloop_report then write('loading bullet types...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_bullet_type(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//bullets
    if s='[FIELD GENERATORS]'then
      begin
      if mainloop_report then write('loading field generators types...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_field_generator_type(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//field generators
    if s='[PARTICLES]'then
      begin
      if mainloop_report then write('loading particles...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_particle_type(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//particles
    if s='[PILOTS]'then
      begin
      if mainloop_report then write('loading pilot types...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        assign(f2,s);
        {$i-}reset(f2);{$i+}
        if ioresult<>0 then writeln('ERROR: file not found:"',s,'"') else
          begin
          load_pilot_type(f2);
          close(f2);
          end;
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//pilots
    if s='[LEVELS]'then
      begin
      if mainloop_report then write('loading levels names...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_level_name(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//levels
    if s='[ASTEROIDS]'then
      begin
      if mainloop_report then write('loading asteroid types...');
      readln(f,s);
      while not eof(f) and(s<>'-')do
        begin
        load_asteroid_type(s);
        readln(f,s);
        end;
      if mainloop_report then writeln('ok');
      end;//asteroids
    end;//while
  close(f);
  end;
end;

procedure load_main_settings(path:ansistring);
var
  f:text;
  s:ansistring;
  b:integer;
begin
assign(f,path);
{$i-}reset(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
  begin
  while not eof(f)do
    begin
    readln(f,s);
    if pos('resx',s)<>0 then val(getval(s,#39),resx);
    if pos('resy',s)<>0 then val(getval(s,#39),resy);
    if pos('soundvol',s)<>0 then val(getval(s,#39),soundvol);
    if pos('gamespeed',s)<>0 then
    	begin
    	val(getval(s,#39),b);
    	gamespeedkoef:=b/100;
    	end;
    if pos('colors',s)<>0 then val(getval(s,#39),kol_colors);
    if pos('showhp',s)<>0 then
      begin
      val(getval(s,#39),b);
      show_hp:=b=1;
      end;
    end;
  close(f);
  end;
end;

procedure save_main_settings(path:ansistring);
var
	f:text;
begin
assign(f,path);
{$i-}rewrite(f);{$i+}
if ioresult<>0 then writeln(logfile,'ERROR: file not found:"',path,'"') else
	begin
  writeln(f,'resx'+#39,resx,#39);
	writeln(f,'resy'+#39,resy,#39);
	writeln(f,'colors'+#39,kol_colors,#39);
	writeln(f,'soundvol'+#39,soundvol,#39);
	writeln(f,'gamespeed'+#39,round(gamespeedkoef*100),#39);
	write(f,'showhp');
	if show_hp then writeln(f,#39+'1'+#39)else writeln(f,#39+'0'+#39);
	close(f);
	end;
end;

procedure load_panel(path:ansistring);
var f:text;
  s:ansistring;
begin
assign(f,path);
reset(f);
with panel do
while not eof(f) do
  begin
  readln(f,s);
  if pos('imgpath',s)<>0 then loadimagefile(itdetect,getval(s,#39),img,0);
  if pos('x',s)<>0 then val(getval(s,#39),x);
  if pos('w',s)<>0 then val(getval(s,#39),w);
  if pos('h',s)<>0 then val(getval(s,#39),h);
  if pos('y1',s)<>0 then val(getval(s,#39),y1);
  if pos('y2',s)<>0 then val(getval(s,#39),y2);
  if pos('y3',s)<>0 then val(getval(s,#39),y3);
  if pos('y4',s)<>0 then val(getval(s,#39),y4);
  end;
close(f);
end;

procedure load_itempanel(path:ansistring);
var f:text;
  s:ansistring;
begin
assign(f,path);
reset(f);
with itempanel do
while not eof(f) do
  begin
  readln(f,s);
  if pos('imgpath',s)<>0 then
    begin
    s:=getval(s,#39);
    loadimagefile(itdetect,s,img_upper,1);
    loadimagefile(itdetect,s,img_lower,2);
    loadimagefile(itdetect,s,img_center,3);
    loadimagefile(itdetect,s,img_full,4);
    end;
  if pos('bar',s)<>0 then
    begin
    s:=str_between(s,'(',')');
    val(get_param(s,1),barx);
    val(get_param(s,2),bary);
    val(get_param(s,3),barw);
    val(get_param(s,4),barh);
    end;
  if pos('item',s)<>0 then
    begin
    s:=str_between(s,'(',')');
    val(get_param(s,1),itmx);
    val(get_param(s,2),itmy);
    end;
  if pos('cutteddy',s)<>0 then val(getval(s,#39),cut_dy);
  end;
close(f);
end;
