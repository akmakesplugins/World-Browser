import "./style.css";
import * as THREE from "three";
import { OrbitControls } from "three/examples/jsm/controls/OrbitControls.js";
import globeMapImg from "/assets/earth.jpg";

/*
  Simple globe:
  - shows a sphere (fallback color)
  - user rotates with mouse (OrbitControls)
  - click on globe computes latitude/longitude
  - sendToHost(...) tries multiple host-bridge fallbacks (cefQuery, webview postMessage, document.title)
*/

const container = document.getElementById("globe");

// small overlay
const coordsEl = document.createElement("div");
coordsEl.id = "coords";
coordsEl.textContent = "Click a location";
container.appendChild(coordsEl);

// Scene + Camera
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(45, container.clientWidth / container.clientHeight, 0.1, 1000);
camera.position.set(0, 0, 3);

// Renderer
const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
renderer.setSize(container.clientWidth, container.clientHeight);
container.appendChild(renderer.domElement);

// Resize handling
window.addEventListener("resize", () => {
    camera.aspect = container.clientWidth / container.clientHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(container.clientWidth, container.clientHeight);
});

// Globe (fallback material so it's visible immediately)
const globeGeometry = new THREE.SphereGeometry(1, 64, 64);
const globeMaterial = new THREE.MeshPhongMaterial({
    shininess: 15,
    specular: 0x333333,
});
const globe = new THREE.Mesh(globeGeometry, globeMaterial);
scene.add(globe);

// Load texture onto globe
const texLoader = new THREE.TextureLoader();
texLoader.load(globeMapImg, (tex) => {
    globe.material.map = tex;
    globe.material.needsUpdate = true;
});

// Add simple lat/long lines (subtle)
const gridGroup = new THREE.Group();
const lineMat = new THREE.LineBasicMaterial({ color: 0xffffff, opacity: 0.15, transparent: true });

for (let lat = -60; lat <= 60; lat += 30) {
    const y = Math.sin(THREE.MathUtils.degToRad(lat));
    const radius = Math.cos(THREE.MathUtils.degToRad(lat));
    const points = [];
    const segments = 128;
    for (let i = 0; i <= segments; i++) {
        const theta = (i / segments) * Math.PI * 2;
        points.push(new THREE.Vector3(Math.cos(theta) * radius, y, Math.sin(theta) * radius));
    }
    const g = new THREE.BufferGeometry().setFromPoints(points);
    const line = new THREE.LineLoop(g, lineMat);
    gridGroup.add(line);
}

for (let lon = 0; lon < 360; lon += 30) {
    const points = [];
    const segments = 64;
    for (let i = 0; i <= segments; i++) {
        const t = (i / segments) * Math.PI - Math.PI / 2;
        const x = Math.cos(t);
        const y = Math.sin(t);
        points.push(new THREE.Vector3(x, y, 0));
    }
    const g = new THREE.BufferGeometry().setFromPoints(points);
    const line = new THREE.Line(g, lineMat);
    line.rotation.y = THREE.MathUtils.degToRad(lon);
    gridGroup.add(line);
}

scene.add(gridGroup);

// Lights
const dirLight = new THREE.DirectionalLight(0xffffff, 1.0);
dirLight.position.set(5, 5, 5);
scene.add(dirLight);
scene.add(new THREE.AmbientLight(0x404040, 0.8));

// OrbitControls (drag only: no auto-rotate, no zoom/pan unless you want it)
const controls = new OrbitControls(camera, renderer.domElement);
controls.enableZoom = false;
controls.enablePan = false;
controls.enableDamping = true;
controls.dampingFactor = 0.08;

// Raycaster for clicks
const raycaster = new THREE.Raycaster();
const pointer = new THREE.Vector2();

function getLatLonFromPoint(point) {
    // point is THREE.Vector3 in world space on sphere surface
    const r = point.length();
    const lat = 90 - (Math.acos(point.y / r) * 180) / Math.PI;
    let lon = (Math.atan2(point.z, point.x) * 180) / Math.PI;
    // normalize lon to -180..180
    if (lon > 180) lon -= 360;
    if (lon < -180) lon += 360;
    return { lat, lon };
}

function onPointerDown(e) {
    // compute normalized device coords (-1..1)
    const rect = renderer.domElement.getBoundingClientRect();
    pointer.x = ((e.clientX - rect.left) / rect.width) * 2 - 1;
    pointer.y = -((e.clientY - rect.top) / rect.height) * 2 + 1;

    raycaster.setFromCamera(pointer, camera);
    const intersects = raycaster.intersectObject(globe, false);

    if (intersects.length > 0) {
        const p = intersects[0].point.clone().normalize(); // on unit sphere
        const { lat, lon } = getLatLonFromPoint(p);
        coordsEl.textContent = `lat: ${lat.toFixed(4)}°, lon: ${lon.toFixed(4)}°`;

        // send to host (JUCE) — see sendToHost implementation below
        sendToHost({ type: "location", lat, lon });
    }
}

renderer.domElement.addEventListener("pointerdown", onPointerDown, false);

// Messaging helper: tries a few host bridges then falls back to title change
function sendToHost(payload) {
    if (window.__JUCE__?.backend?.emitEvent) {
        window.__JUCE__.backend.emitEvent("locationClicked", payload);
    } else {
        console.log("JUCE backend not available", payload);
    }
}

// Optional: load a texture (commented out — you said you'd prefer custom globe)
// If you later want to use a texture, uncomment and set a local file or CDN path.
// const texLoader = new THREE.TextureLoader();
// texLoader.load('/assets/earth.jpg', tex => {
//   globe.material.map = tex;
//   globe.material.needsUpdate = true;
// });

// Main render loop
function animate() {
    requestAnimationFrame(animate);
    controls.update();
    renderer.render(scene, camera);
}
animate();
