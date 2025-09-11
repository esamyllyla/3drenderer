(() => {

let app = document.getElementById("app");
app.width = 800;
app.height = 600;
let ctx = app.getContext("2d");
let w = null;

let mouseX = 0;
let mouseY = 0;
let isMouseDown = 0;
let prevMouseDown = 0;
let mouseInit = 0;

app.addEventListener("mousemove", e => {
	mouseX = e.offsetX;
	mouseY = e.offsetY;
});

app.addEventListener("mousedown", e => {
	if(e.button === 0) {
		isMouseDown = 1;
	}
});

app.addEventListener("mouseup", e => {
	if(e.button === 0) {
		isMouseDown = 0;
	}
});

function make_environment(...envs)
{
	return new Proxy(envs, 
	{
		get(target, prop, receiver) 
		{
			for(let env of envs) 
			{
				if(env.hasOwnProperty(prop)) 
				{
					return env[prop];
				}
			}
		return (...args) => {console.error("NOT IMPLEMENTED: "+prop, args)}
		}
	});
}

WebAssembly.instantiateStreaming(fetch('wasm.wasm'),{
	"env": make_environment({
		"atan2f": Math.atan2,
		"cosf": Math.cos,
		"sinf": Math.sin,
		"tanf": Math.tan,
		"powf": Math.pow,
		"debugToLogValue0": (val) => {
			console.log("Leaveangle: ", val);
		},
		"debugToLogValue1": (val) => {
			console.log("LeaveangleY: ", val);
		},
})
}).then(w0 => {
	w = w0;

	let prev = null;
	function first(timestamp)
	{
		prev = timestamp;
		window.requestAnimationFrame(loop);
	}
	function loop(timestamp)
	{
	if(isMouseDown && !prevMouseDown)
	{
		mouseInit = mouseX;
	}

	if(isMouseDown && prevMouseDown)
	{
		//console.log("mouse init: " + mouseInit);
		//console.log("mouseX: " + mouseX);
		//console.log("movement: " + (mouseX - mouseInit));
	}
		const dt = timestamp - prev;
		prev = timestamp;
		const buffer = w.instance.exports.memory.buffer;
		const pixels = w.instance.exports.render(0.001, isMouseDown, prevMouseDown, mouseX, mouseY);
		prevMouseDown = isMouseDown;
		const image = new ImageData(new Uint8ClampedArray(buffer, pixels, app.width*app.height*4), app.width)
		ctx.putImageData(image, 0, 0);
		window.requestAnimationFrame(loop);	
	}
	window.requestAnimationFrame(first);
})

})();
