const
  MAX_SOUNDS=15;
type
  Tsound=record
    channel:Hstream;
    looping:boolean;
    path:string[50];
    end;
  Psound=^Tsound;
var
  sound:array[0..MAX_SOUNDS-1]of Tsound;
function sound_freesoundindx:integer;
var i:integer;
begin
sound_freesoundindx:=-1;
i:=0;
while (sound_freesoundindx=-1)and(i<MAX_SOUNDS)do
  begin
  if sound[i].channel=-1 then sound_freesoundindx:=i;
  inc(i);
  end;
end;

function add_sound(path:ansistring;loop:boolean;volu:boolean;x0,y0:double):Psound;
var vol:double;
  indx:integer;
begin
add_sound:=nil;
if sound_freesoundindx<>-1 then
  begin
  indx:=sound_freesoundindx;
  add_sound:=@sound[indx];
  sound[indx].channel:=bass_streamcreatefile(false,pchar(path),0,0,0);
  bass_channelplay(sound[indx].channel,true);
  sound[indx].looping:=loop;
  sound[indx].path:=path;
{  if volu and(plship<>nil) then
    begin
    dxs_setvolume(dxs_sound[indx],1-sqrt(sqr(x0-plship^.x)+sqr(y0-plship^.y))/(SOUNDSILENCE_DIST*getmaxy));
    end;}
  end;
end;

procedure sound_checksounds;
var i:integer;
begin
for i:=0 to MAX_SOUNDS-1 do
  if bass_channelisactive(sound[i].channel)<>bass_active_playing then
    begin
    bass_streamfree(sound[i].channel);
    sound[i].channel:=-1;
    if sound[i].looping then add_sound(sound[i].path,true,false,0,0);
    end;
end;

procedure sound_destroysounds;
var i:integer;
begin
//bass_free;
for i:=0 to MAX_SOUNDS-1 do sound[i].channel:=-1;
end;

procedure remove_sound(var snd:Psound);
begin
if snd<>nil then
  begin
  bass_streamfree(snd^.channel);
  snd^.channel:=-1;
  snd:=nil;
  end;
end;
