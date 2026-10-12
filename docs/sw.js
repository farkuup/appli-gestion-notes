// Service worker : garde les fichiers de l'appli en mémoire pour qu'elle marche sans réseau.
// Quand tu changes un fichier de l'appli, augmente le numéro de VERSION.
const VERSION = "sonote-v1";
const FICHIERS = [
  "./", "index.html", "sonote.js", "sonote.wasm", "structure.txt",
  "manifest.webmanifest", "icon-192.png", "icon-512.png"
];

self.addEventListener("install", e => {
  e.waitUntil(caches.open(VERSION).then(c => c.addAll(FICHIERS)).then(() => self.skipWaiting()));
});

self.addEventListener("activate", e => {
  e.waitUntil(
    caches.keys()
      .then(noms => Promise.all(noms.filter(n => n !== VERSION).map(n => caches.delete(n))))
      .then(() => self.clients.claim())
  );
});

// D'abord le cache, sinon le réseau.
self.addEventListener("fetch", e => {
  if (e.request.method !== "GET") return;
  e.respondWith(caches.match(e.request).then(r => r || fetch(e.request)));
});
