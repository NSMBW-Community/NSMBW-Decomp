const indProgContainer = data => `
<div class="ind-progress-container p${data.num} gamefont">
    <span class="sf-name">${data.name}</span>
    <span class="byte-count">Code bytes done:<br>${data.doneBytes}/${data.totalBytes}</span>
    <span class="add-count">Since last commit:<br>+${data.percChange.toFixed(3)}%</span>
    <div class="prog-cont">
        <span class="prog-val-title">SCORE</span>
        <span class="prog-val">${data.perc.toFixed(3)}%</span>
    </div>
    <div class="filler-line"></div>
</div>
`;

fetch("progress.csv").then(r => r.text()).then(r => {
    let data = r.trim().replace(/\r/g, "").split("\n").map(e => {
        const split = e.match(/[^,]+/g);
        return {
            "ts": new Date(split[0] * 1000),
            "commit_hash": split[1],
            "wiimj2d.dol": [parseInt(split[2]), parseInt(split[3])],
            "d_profileNP.rel": [parseInt(split[4]), parseInt(split[5])],
            "d_basesNP.rel": [parseInt(split[6]), parseInt(split[7])],
            "d_enemiesNP.rel": [parseInt(split[8]), parseInt(split[9])],
            "d_en_bossNP.rel": [parseInt(split[10]), parseInt(split[11])],
        };
    });
    configureOverview(data);
    configureGraph(data);
});

function configureOverview(csvData) {
    const newestEntry = csvData[csvData.length - 1];
    const secondNewestEntry = csvData[csvData.length - 2];
    const names = ["wiimj2d.dol", "d_profileNP.rel", "d_basesNP.rel", "d_enemiesNP.rel", "d_en_bossNP.rel"];
    let htmls = [];
    for (const name of names) {
        const done = parseInt(newestEntry[name][0]);
        const total = parseInt(newestEntry[name][1]);
        const lastDone = parseInt(secondNewestEntry[name][0]);
        const lastTotal = parseInt(secondNewestEntry[name][1]);
        htmls.push(indProgContainer({
            name: name,
            num: htmls.length + 1,
            doneBytes: done,
            totalBytes: total,
            percChange: ((done / total) - (lastDone / lastTotal)) * 100,
            perc: (done / total) * 100
        }));
    }
    document.getElementById("prog-conts-container").innerHTML = htmls.join("");
}

function nextMonth(date) {
    const tmp = (date.getMonth() == 1) ? [0, date.getFullYear() + 1] : [date.getMonth() + 1, date.getFullYear()];
    const newDate = new Date(date);
    newDate.setMonth(tmp[0], 1);
    newDate.setYear(tmp[1]);
    return newDate;
}

function configureGraph(csvData) {
    const canvas = document.getElementById("prog-graph");
    /** @type {CanvasRenderingContext2D} */
    const ctx = canvas.getContext("2d");
    ctx.canvas.width = 2000;
    ctx.canvas.height = 1400;
    const cw = canvas.width;
    const ch = canvas.height;
    const paddingL = 0.1;
    const paddingR = 0.97;
    const paddingU = 0.05;
    const paddingD = 0.95;
    const paddingW = paddingL + (1 - paddingR);
    const paddingH = paddingU + (1 - paddingD);

    ctx.textBaseline = "middle";
    ctx.font = "40px NSMBW";

    ctx.clearRect(0, 0, cw, ch);

    ctx.fillStyle = "#eee";
    ctx.strokeStyle = "#bbb";
    ctx.beginPath();
    // y axes
    ctx.textAlign = "right";
    for (let i = 0; i <= 100; i += 10) {
        const y = (ch * paddingU) + (ch * (1 - paddingH) / 100) * i;
        ctx.moveTo(cw * paddingL, y);
        ctx.lineTo(cw * paddingR, y);
        ctx.fillText((100 - i) + "%", cw * paddingL * 0.9, y);
    }
    // x axes
    ctx.textAlign = "center";
    ctx.textBaseline = "top";
    const begin = csvData[0].ts;
    const end = csvData[csvData.length - 1].ts;

    // Round up to nearest month
    const beginMY = nextMonth(begin);

    for (let i = beginMY; i <= end; i = nextMonth(i)) {
        const frac = (i - begin) / (end - begin);
        const x = (cw * paddingL) + (cw * (1 - paddingW)) * frac;
        ctx.moveTo(x, ch * paddingU);
        ctx.lineTo(x, ch * paddingD);
        const dateFormatted = i.toLocaleString("default", {
            month: "short",
            year: "2-digit"
        });
        ctx.fillText(dateFormatted, x, ch * paddingD + 10);
    }
    ctx.stroke();

    ctx.beginPath();
    // Bold zero y- and x-axis
    ctx.lineWidth = 2;
    ctx.strokeStyle = "#eee";
    ctx.moveTo(cw * paddingL, ch * paddingU);
    ctx.lineTo(cw * paddingL, ch * paddingD + 15);
    ctx.moveTo(cw * paddingL - 15, ch * paddingD);
    ctx.lineTo(cw * paddingR, ch * paddingD);
    ctx.stroke();
    

    // Draw points
    let pointList = [];
    for (const progPoint of csvData) {
        const timeFrac = (progPoint.ts.getTime() - begin) / (end - begin);
        const x = (cw * paddingL) + (cw * (1 - paddingW)) * timeFrac;
        const progFrac = progPoint["wiimj2d.dol"][0] / progPoint["wiimj2d.dol"][1];
        const y = (ch * paddingU) + ch * (1 - paddingH) * (1 - progFrac);
        pointList.push([x, y]);
    }
    // line
    ctx.strokeStyle = "#d4b32488";
    ctx.lineWidth = 5;

    ctx.beginPath();
    ctx.moveTo(pointList[0][0], pointList[0][1]);
    for (let i = 1; i < pointList.length; i++) {
        ctx.lineTo(pointList[i][0], pointList[i][1]);
    }
    ctx.stroke();

    ctx.fillStyle = "#d1ad0f88";
    ctx.beginPath();
    ctx.moveTo(pointList[0][0], pointList[0][1]);
    for (let i = 1; i < pointList.length; i++) {
        ctx.lineTo(pointList[i][0], pointList[i][1]);
    }
    ctx.lineTo(cw * paddingR, ch * paddingD);
    ctx.lineTo(cw * paddingL, ch * paddingD);
    ctx.fill();

    ctx.strokeStyle = "#d4b324";
    ctx.fillStyle = "#d1ad0f88";
    for (const point of pointList) {
        ctx.beginPath();
        ctx.arc(point[0], point[1], 8, 0, 2 * Math.PI);
        ctx.fill();
        ctx.stroke();
    }

    //requestAnimationFrame(() => configureGraph(csvData));
}

const btnAbout = document.getElementById("btn-about");
const btnProgress = document.getElementById("btn-progress");
btnAbout.addEventListener("click", () => {
    document.getElementById("main-container-about").style.display = "";
    document.getElementById("main-container-progress").style.display = "none";
    btnAbout.blur();
});
btnProgress.addEventListener("click", () => {
    document.getElementById("main-container-about").style.display = "none";
    document.getElementById("main-container-progress").style.display = "";
    btnProgress.blur();
});
