const indProgContainer = data => {
    let percStr = (data.perc >= 100) ? "100%" : data.perc.toFixed(3) + "%";
    return `
<div class="ind-progress-container p${data.num} gamefont">
    <span class="sf-name">${data.name}</span>
    <span class="byte-count">Code bytes done:<br>${data.doneBytes}/${data.totalBytes}</span>
    <span class="add-count">Since last commit:<br>+${data.percChange.toFixed(3)}%</span>
    <div class="prog-cont">
        <span class="prog-val-title">SCORE</span>
        <span class="prog-val">${percStr}</span>
    </div>
    <div class="filler-line"></div>
</div>
`;
};

const cw = 1000;
const ch = 700;
const paddingL = 0.1;
const paddingR = 0.97;
const paddingU = 0.05;
const paddingD = 0.95;
const paddingW = paddingL + (1 - paddingR);
const paddingH = paddingU + (1 - paddingD);

const sfNames = ["wiimj2d.dol", "d_profileNP.rel", "d_basesNP.rel", "d_en_bossNP.rel"];

const svg = document.getElementById("prog-graph-svg");

let globalCsvData, globalInfoLine;

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
    globalCsvData = data;
    configureOverview(data);
    configureGraph(data);
    globalCsvData = data;
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

function svgAddAttrs(el, attrs) {
    if (attrs) {
        for (const attr in attrs) {
            el.setAttribute(attr, attrs[attr]);
        }
    }
}
function svgAddStyles(el, styles) {
    if (styles) {
        for (const style in styles) {
            el.style[style] = styles[style];
        }
    }
}
function svgLine(x1, y1, x2, y2, stroke, strokeWeight, attrs, styles) {
    let el = document.createElementNS("http://www.w3.org/2000/svg", "line");
    el.setAttribute("x1", x1);
    el.setAttribute("y1", y1);
    el.setAttribute("x2", x2);
    el.setAttribute("y2", y2);
    svgAddAttrs(el, attrs);
    el.style.stroke = stroke;
    el.style.strokeWidth = strokeWeight;
    return el;
};
function svgText(content, x, y, fill, stroke, strokeWeight, attrs, styles) {
    let el = document.createElementNS("http://www.w3.org/2000/svg", "text");
    el.innerHTML = content;
    el.setAttribute("x", x);
    el.setAttribute("y", y);
    svgAddAttrs(el, attrs);
    el.style.stroke = stroke;
    el.style.fill = fill;
    el.style.strokeWidth = strokeWeight;
    svgAddStyles(el, styles);
    return el;
};
function svgCircle(r, x, y, fill, stroke, strokeWeight, attrs, styles) {
    let el = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    el.setAttribute("r", r);
    el.setAttribute("cx", x);
    el.setAttribute("cy", y);
    svgAddAttrs(el, attrs);
    el.style.stroke = stroke;
    el.style.fill = fill;
    el.style.strokeWidth = strokeWeight;
    return el;
};

function configureGraph(csvData) {
    // y axes
    for (let i = 0; i <= 100; i += 10) {
        const y = (ch * paddingU) + (ch * (1 - paddingH) / 100) * i;
        svg.append(svgLine(cw * paddingL, y, cw * paddingR, y, "#bbb", 1));
        const percText = (100 - i) + "%";
        const textAttrs = { "text-anchor": "end", "dominant-baseline": "central" };
        svg.append(svgText(percText, cw * paddingL * 0.9, y, "black", "black", 6, textAttrs));
        svg.append(svgText(percText, cw * paddingL * 0.9, y, "white", "white", 0, textAttrs));
    }

    // x axes
    const begin = csvData[0].ts;
    const end = csvData[csvData.length - 1].ts;

    const beginMY = nextMonth(begin); // Round up to nearest month

    for (let i = beginMY; i <= end; i = nextMonth(i)) {
        const frac = (i - begin) / (end - begin);
        const x = (cw * paddingL) + (cw * (1 - paddingW)) * frac;
        svg.append(svgLine(x, ch * paddingU, x, ch * paddingD, "#bbb", 1));
        const dateFormatted = i.toLocaleString("default", {
            month: "short",
            year: "numeric"
        });
        const textAttrs = { "text-anchor": "end", "dominant-baseline": "hanging" };
        svg.append(svgText(dateFormatted, x, ch * paddingD + 10, "black", "black", 5, textAttrs, { "font-size": "1.6em" }));
        svg.append(svgText(dateFormatted, x, ch * paddingD + 10, "white", "white", 0, textAttrs, { "font-size": "1.6em" }));
    }

    // Bold zero y axis
    svg.append(svgLine(cw * paddingL, ch * paddingU, cw * paddingL, ch * paddingD, "#eee", 2));

    // Info line
    globalInfoLine = svgLine(-9999, ch * paddingU, -9999, ch * paddingD, "#96d932d1", 3);
    svg.append(globalInfoLine);

    // Draw points, add XY coords to csvData
    for (let i = 0; i < csvData.length; i++) {
        const progPoint = csvData[i];
        const timeFrac = (progPoint.ts.getTime() - begin) / (end - begin);
        const x = (cw * paddingL) + (cw * (1 - paddingW)) * timeFrac;
        const doneAllSFs = sfNames.reduce((a, v) => a + progPoint[v][0], 0);
        const totalAllSFs = sfNames.reduce((a, v) => a + progPoint[v][1], 0);
        const progFrac = doneAllSFs / totalAllSFs;
        const y = (ch * paddingU) + ch * (1 - paddingH) * (1 - progFrac);
        globalCsvData[i].totalFrac = progFrac;
        globalCsvData[i].x = x;
        globalCsvData[i].y = y;
        globalCsvData[i].prev = i >= 1 ? globalCsvData[i - 1] : globalCsvData[0];
    }

    // Lines between points
    for (let i = 0; i < csvData.length; i++) {
        svg.append(svgCircle(5, csvData[i].x, csvData[i].y, "#d1ad0f88", "#d4b324", 2));
        if (i >= 1) {
            svg.append(svgLine(csvData[i - 1].x, csvData[i - 1].y, csvData[i].x, csvData[i].y, "#d4b32499", 3));
        }
    }

    // Info line starts at latest commit
    mouseMove();

    const callMouseMoveEv = ev => {
        let bounds = svg.getBoundingClientRect();
        let x = (ev.clientX - bounds.left) / svg.clientWidth * cw;
        let y = (ev.clientY - bounds.top) / svg.clientHeight * ch;
        mouseMove(x, y);
    };

    svg.addEventListener("mousemove", callMouseMoveEv);
    svg.addEventListener("touchmove", ev => callMouseMoveEv(ev.targetTouches.item(0)));
}

let prevNearest;
function mouseMove(x, y) {
    if (!globalCsvData || !globalInfoLine) return;
    let nearestPoint;
    if (x == undefined) {
        if (prevNearest == undefined) {
            nearestPoint = globalCsvData[globalCsvData.length - 1];
        } else {
            nearestPoint = prevNearest;
        }
    } else if (x == Infinity) {
        nearestPoint = globalCsvData[globalCsvData.length - 1];
    } else {
        nearestPoint = globalCsvData.reduce((p, c) => (Math.abs(c.x - x) < Math.abs(p.x - x)) ? c : p, globalCsvData[0]);
    }
    prevNearest = nearestPoint;
    const commitInfobox = document.getElementById("commit-info");
    const infoboxContent = `
        <span>Commit <a href="https://github.com/CLF78/NSMBW-Decomp/commit/${nearestPoint.commit_hash}">${nearestPoint.commit_hash.substring(0, 6)}</a></span>
        <span style="margin-bottom: 10px;">${nearestPoint.ts.toLocaleString().replace(",", "")}</span>
        <span id="infobox-perc">${(nearestPoint.totalFrac * 100).toFixed(3)}%</span>
    `;
    if (commitInfobox.innerHTML != infoboxContent) {
        commitInfobox.innerHTML = infoboxContent;
    }

    const bounds = commitInfobox.getBoundingClientRect();
    const ratioW = svg.clientWidth / cw;
    const ratioH = svg.clientHeight / ch;
    let leftVal, topVal;
    if (nearestPoint.x - bounds.width / 2 < cw * paddingL) {
        leftVal = cw * paddingL * ratioW;
    } else if (nearestPoint.x + bounds.width / 2 > cw * paddingR) {
        leftVal = cw * paddingR * ratioW - bounds.width;
    } else {
        leftVal = (nearestPoint.x / cw) * svg.clientWidth - bounds.width / 2;
    }
    if (nearestPoint.y - bounds.height < ch * paddingU) {
        topVal = ch * paddingU * ratioH + 10;
    } else {
        topVal = nearestPoint.y * ratioH - bounds.height - 10;
    }

    commitInfobox.style.left = leftVal + "px";
    commitInfobox.style.top = topVal + "px";

    globalInfoLine.setAttribute("x1", nearestPoint.x);
    globalInfoLine.setAttribute("x2", nearestPoint.x);
}

function showAbout() {
    document.getElementById("main-container-about").style.display = "";
    document.getElementById("main-container-progress").style.display = "none";
}
function showProgress() {
    document.getElementById("main-container-about").style.display = "none";
    document.getElementById("main-container-progress").style.display = "";
    mouseMove();
}

const btnProgress = document.getElementById("btn-progress");
btnProgress.addEventListener("click", e => {
    history.pushState("", "", "#progress");
    e.preventDefault();
    showProgress();
    btnProgress.blur();
});

if (location.hash == "#progress") {
    showProgress();
}

window.addEventListener("resize", () => mouseMove());

window.addEventListener("", () => {
    document.getElementById("main-container-about").style.display = "";
    document.getElementById("main-container-progress").style.display = "none";
});
window.addEventListener("popstate", e => {
    if (location.hash == "#progress") {
        showProgress();
    } else {
        showAbout();
    }
});
