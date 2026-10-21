#!/usr/bin/env python3
"""Build a browsable, static PNG preview of the SpaceWarrior graphics."""
from __future__ import annotations

import html
import json
import re
from io import BytesIO
from pathlib import Path
from collections import Counter

from PIL import Image, ImageOps, ImageChops

ROOT = Path(__file__).resolve().parents[1]
INPUT = ROOT / "gfx"
DEST = ROOT / "gfx-preview"
ASSETS = DEST / "assets"

# These are not animations: the game loads first frame as RGB and second as mask.
EMBEDDED = set("""
cursor.gif
fld_gen/banan.gif
fld_gen/large/banan.gif
other/left_arrow.gif
other/loadmenu.gif
other/menu.gif
other/right_arrow.gif
other/savemenu.gif
particles/small_red.gif
particles/smoke-blue.gif
particles/smoke-red.gif
particles/smoke1.gif
particles/smoke2.gif
particles/smoke3.gif
weapons/ray1.gif
weapons/rocket1.gif
weapons/rocket2.gif
weapons/large/ray1.gif
weapons/large/rocket1.gif
weapons/large/rocket2.gif
""".split())

def save(im, target):
    target.parent.mkdir(parents=True, exist_ok=True)
    im.save(target, format="PNG", optimize=True)

def lookup_mask(p, files):
    for suffix in ("_mask", "mask"):
        candidate = p.with_name(p.stem + suffix + ".gif")
        if candidate in files:
            return candidate
    return None

def alpha_from_mask(frame):
    # Black=transparent, white=opaque. Respect GIF transparency too:
    # otherwise discarded frame pixels may become opaque white boxes.
    rgba = frame.convert("RGBA")
    luminance = ImageOps.grayscale(rgba.convert("RGB"))
    return ImageChops.multiply(luminance, rgba.getchannel("A"))

def open_mask_gif(path):
    """GIF disposal can fill outside the current frame with an opaque WHITE
    global background. On ship1/ship2 masks it produces ghostly rectangles
    whose positions change with rotation. A mask needs a black background.
    Change only the logical-screen background INDEX, not the sprite palette.
    """
    data = bytearray(path.read_bytes())
    if data[:6] not in (b"GIF87a", b"GIF89a"):
        raise ValueError(f"Not a GIF: {path}")
    data[11] = 0   # global palette entry 0 is black in these masks
    return Image.open(BytesIO(data))

def make():
    ASSETS.mkdir(parents=True, exist_ok=True)
    gifs = sorted(INPUT.rglob("*.gif"))
    others = sorted(list(INPUT.rglob("*.jpg")) + list(INPUT.rglob("*.png")))
    gifset = set(gifs)
    matched = {}
    used_masks = set()
    warnings = []
    for p in gifs:
        if p.stem.lower().endswith(("_mask", "mask")):
            continue
        m = lookup_mask(p, gifset)
        if m is None:
            continue
        with Image.open(p) as color, Image.open(m) as mask:
            if color.n_frames == mask.n_frames and color.size == mask.size:
                matched[p] = m
                used_masks.add(m)
            else:
                warnings.append("Incompatible external mask (exported separately): " + str(m.relative_to(INPUT)))
    cards = []
    total_png = 0
    for p in gifs:
        if p in used_masks:
            continue
        rel = p.relative_to(INPUT)
        key = rel.with_suffix("")
        img_target = ASSETS / key.with_suffix(".png")
        mask_path = matched.get(p)
        embedded = rel.as_posix() in EMBEDDED
        durations = []
        with Image.open(p) as gif:
            frame_count = gif.n_frames
            if embedded and (frame_count != 2 or mask_path is not None):
                warnings.append("Embedded mask did not have 2 frames: " + rel.as_posix())
                embedded = False
            external = open_mask_gif(mask_path) if mask_path else None
            try:
                frames = []
                if embedded:
                    gif.seek(0)
                    color = gif.convert("RGBA").copy()
                    gif.seek(1)
                    mask = alpha_from_mask(gif)
                    color.putalpha(mask)
                    frames.append(color)
                    durations.append(0)
                else:
                    for i in range(frame_count):
                        gif.seek(i)
                        frame = gif.convert("RGBA").copy()
                        durations.append(max(50, gif.info.get("duration", 100)))
                        if external is not None:
                            external.seek(i)
                            # Preserve transparency encoded in the color GIF too.
                            frame.putalpha(ImageChops.multiply(
                                frame.getchannel("A"), alpha_from_mask(external)))
                        frames.append(frame)
                save(frames[0], img_target)
                total_png += 1
                # A genuine animation: keep individual frames for inspection.
                frame_urls = []
                if len(frames) > 1:
                    for i, frame in enumerate(frames):
                        path = ASSETS / key / ("%03d.png" % i)
                        save(frame, path)
                        total_png += 1
                        frame_urls.append(path.relative_to(DEST).as_posix())
                cards.append(dict(
                    name=rel.as_posix(),
                    category=rel.parts[0] if len(rel.parts) > 1 else "root",
                    image=img_target.relative_to(DEST).as_posix(),
                    frames=frame_urls,
                    delays=durations,
                    size=list(gif.size),
                    embedded_mask=embedded,
                    external_mask=str(mask_path.relative_to(INPUT)) if mask_path else None,
                    source_frames=frame_count
                ))
                print("  ", rel.as_posix(), "->", len(frames), "PNG frame(s)", flush=True)
            finally:
                if external:
                    external.close()
    for p in others:
        rel = p.relative_to(INPUT)
        target = ASSETS / rel.with_suffix(".png")
        with Image.open(p) as im:
            save(im.convert("RGBA"), target)
            wh = im.size
        total_png += 1
        cards.append(dict(name=rel.as_posix(), category=rel.parts[0] if len(rel.parts) > 1 else "root",
                          image=target.relative_to(DEST).as_posix(), frames=[],
                          delays=[], size=list(wh), embedded_mask=False, external_mask=None,
                          source_frames=1))
    manifest = dict(
        source="https://github.com/mofr/SpaceWarrior/tree/master/gfx",
        gif_files=len(gifs), other_files=len(others), png_files=total_png,
        paired_masks=len(used_masks), embedded_masks=sum(c["embedded_mask"] for c in cards),
        warnings=warnings, assets=cards
    )
    (DEST / "manifest.json").write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding="utf8")
    generate_html(manifest)
    print("DONE: ", manifest["png_files"], "PNGs,", len(cards), "assets,", len(warnings), "warnings")

def generate_html(manifest):
    data = json.dumps(manifest["assets"], ensure_ascii=False).replace("<", "\\u003c")
    count = len(manifest["assets"])
    categories = dict(Counter(c["category"] for c in manifest["assets"]))
    page = r"""<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>SpaceWarrior — sprite gallery</title>
<style>
:root{color-scheme:dark;--bg:#0d0f17;--panel:#171b28;--border:#2c3345;--mute:#939bb1;--hi:#89dafa}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:#e7ebf4;font:14px/1.5 system-ui,-apple-system,Segoe UI,sans-serif}
header{padding:30px clamp(16px,4vw,70px) 16px;position:sticky;top:0;z-index:5;background:linear-gradient(#0d0f17 90%,#0d0f17ef);border-bottom:1px solid var(--border)}
h1{margin:0;font-size:clamp(25px,3vw,40px);letter-spacing:-.04em;font-weight:780}h1 span{color:var(--hi)}
.sub{color:var(--mute);margin:3px 0 18px}.toolbar{display:flex;align-items:center;gap:12px;flex-wrap:wrap}.toolbar input[type=search]{background:#202538;border:1px solid var(--border);color:white;border-radius:10px;padding:11px 14px;min-width:210px;flex:1;outline:0;font:inherit}
.toolbar input:focus{border-color:var(--hi)}label{color:var(--mute);display:flex;gap:8px;align-items:center;white-space:nowrap}
input[type=range]{width:110px;accent-color:var(--hi)}button{color:#bec7db;background:#202538;border:1px solid var(--border);padding:7px 11px;border-radius:20px;cursor:pointer;font:inherit}button.active{background:#224357;color:#bceeff;border-color:#39708a}
#filters{display:flex;gap:8px;overflow-x:auto;padding-top:13px;padding-bottom:5px}#counter{color:var(--mute);margin:18px 0}
main{padding:0 clamp(16px,4vw,70px) 100px}
.gallery{display:grid;grid-template-columns:repeat(auto-fill,minmax(var(--tile,210px),1fr));gap:14px}
.card{overflow:hidden;border:1px solid var(--border);border-radius:13px;background:var(--panel);min-width:0;content-visibility:auto;contain-intrinsic-size:auto 260px}
.visual{height:175px;display:flex;align-items:center;justify-content:center;position:relative;overflow:hidden;background-color:#222734;background-image:linear-gradient(45deg,#252b3a 25%,transparent 25%),linear-gradient(-45deg,#252b3a 25%,transparent 25%),linear-gradient(45deg,transparent 75%,#252b3a 75%),linear-gradient(-45deg,transparent 75%,#252b3a 75%);background-size:24px 24px;background-position:0 0,0 12px,12px -12px,-12px 0px}
.visual img{max-width:90%;max-height:90%;object-fit:contain;image-rendering:pixelated;filter:drop-shadow(0 4px 8px #0008)}
body.smooth .visual img{image-rendering:auto}
.meta{padding:11px 13px 13px}.name{font-size:12px;overflow-wrap:anywhere;line-height:1.35;font-weight:660;min-height:2.7em;margin-bottom:7px}
.tags{font-size:11px;display:flex;gap:7px;flex-wrap:wrap;color:var(--mute)}.tag{padding:2px 7px;border-radius:5px;background:#252b3c}

.visual a{position:absolute;inset:0;z-index:1}footer{color:var(--mute);padding:28px 0;opacity:.8}footer a{color:var(--hi)}
@media(max-width:550px){header{position:static}.visual{height:145px}.gallery{grid-template-columns:repeat(2,minmax(0,1fr));gap:9px}.meta{padding:9px}.name{font-size:11px}}
</style></head><body>
<header><h1>Space<span>Warrior</span> / gfx</h1>
<p class="sub">Browse the original sprites, masks merged into transparent PNGs. Hover animated tiles to play.</p>
<div class="toolbar">
<input type="search" id="query" placeholder="Search files…" aria-label="Search assets">
<label>Tile size <input id="size" type="range" min="150" max="360" step="10" value="210"></label>
<label><input type="checkbox" id="smooth"> Smooth scaling</label>
</div><nav id="filters"></nav></header>
<main><div id="counter"></div><div class="gallery" id="gallery"></div>
<footer>Preserves the original graphics; <a href="manifest.json">manifest / metadata</a> · <a href="https://github.com/mofr/SpaceWarrior">source repository</a></footer>
</main>
<script>
const assets=/* DATA */;
const gallery=document.getElementById("gallery");
const counter=document.getElementById("counter");
const filters=document.getElementById("filters");
const query=document.getElementById("query");
let active="all";
const animatedCards = new Set();
const viewportObserver = new IntersectionObserver(entries=>{
 for(const entry of entries){
  const card=entry.target;
  if(entry.isIntersecting && !document.hidden)card.startAnimation?.();
  else card.stopAnimation?.();
 }
},{rootMargin:"100px"});
const categories=["all",...new Set(assets.map(a=>a.category))];
categories.forEach(cat=>{
 const b=document.createElement("button");b.textContent=cat==="all"?"All":cat;b.dataset.cat=cat;
 b.onclick=()=>{active=cat;updateFilters();render()};filters.appendChild(b);
});
function updateFilters(){filters.querySelectorAll("button").forEach(b=>b.classList.toggle("active",b.dataset.cat===active))}
function render(){
 for(const card of animatedCards){card.stopAnimation?.();viewportObserver.unobserve(card)}
 animatedCards.clear();
 const q=query.value.trim().toLowerCase();
 const entries=assets.filter(a=>(active==="all"||a.category===active)&&a.name.toLowerCase().includes(q));
 counter.textContent=entries.length+" items · "+assets.length+" total";
 gallery.replaceChildren();
 const fragment=document.createDocumentFragment();
 for(const a of entries){
  const card=document.createElement("article");card.className="card";
  const visual=document.createElement("div");visual.className="visual";
  const link=document.createElement("a");link.href=a.image;link.target="_blank";link.title="Open original PNG";
  const img=document.createElement("img");img.src=a.image;img.loading="lazy";img.alt=a.name;
  visual.append(img,link);
  const meta=document.createElement("div");meta.className="meta";
  const name=document.createElement("div");name.className="name";name.textContent=a.name;
  const tags=document.createElement("div");tags.className="tags";
  const tag=(s)=>{const n=document.createElement("span");n.className="tag";n.textContent=s;tags.append(n)};
  tag(a.size.join("×"));if(a.frames.length)tag(a.frames.length+" frames");
  if(a.embedded_mask)tag("embedded alpha");
  meta.append(name,tags);
  if(a.frames.length){
   let idx=0, direction=1, tick=null;
   const show=()=>{
    const src=a.frames[idx];
    if(!img.src.endsWith(src))img.src=src;
    link.href=src;
   };
   const advance=()=>{
    const bounce=(a.category==="ships"||a.name==="other/gate.gif");
    if(bounce && a.frames.length>2){
     if(idx===a.frames.length-1)direction=-1;
     else if(idx===0)direction=1;
     idx+=direction;
    }else idx=(idx+1)%a.frames.length;
    show();
   };
   card.startAnimation=()=>{
    if(tick===null){
     show();
     tick=setInterval(advance,Math.max(70,a.delays[idx]||100));
    }
   };
   card.stopAnimation=()=>{if(tick!==null){clearInterval(tick);tick=null}};
   animatedCards.add(card);
  }
  card.append(visual,meta);fragment.append(card);
  if(a.frames.length)viewportObserver.observe(card);
 }
 gallery.append(fragment);
}
document.addEventListener("visibilitychange",()=>{
 for(const card of animatedCards){
  if(document.hidden)card.stopAnimation?.();
  else if(card.getBoundingClientRect().bottom>=-100 && card.getBoundingClientRect().top<=innerHeight+100)card.startAnimation?.();
 }
});
query.addEventListener("input",render);
document.getElementById("size").addEventListener("input",e=>document.documentElement.style.setProperty("--tile",e.target.value+"px"));
document.getElementById("smooth").addEventListener("change",e=>document.body.classList.toggle("smooth",e.target.checked));
updateFilters();render();
</script></body></html>"""
    page = page.replace("/* DATA */", data)
    (DEST / "index.html").write_text(page, encoding="utf8")

if __name__ == "__main__":
    make()
