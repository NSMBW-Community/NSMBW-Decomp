
////////////////////
// Custom Bubbles //
////////////////////

function createCustomBubble(div, labelContent, shortText, extendedText) {

    // Get inner text
    if (div.innerHTML) {
        extendedText += ' ' + div.innerHTML;
    }

    // For functions and members
    if (div.parentElement.className === 'memdoc') {
        const memproto = div.parentElement.closest('.memitem').querySelector('.memproto');

        const label = document.createElement('span');
        label.title = extendedText
        label.classList.add('mlabel', 'customlabel');
        label.innerText = labelContent;

        // We need to set up the label table if not present
        if (memproto.firstElementChild.className === 'memname') {
            const innerTable = memproto.firstElementChild;
            const table = memproto.appendChild(document.createElement('table'));
            table.className = 'mlabels';

            const tbody = table.appendChild(document.createElement('tbody'));
            const tr = tbody.appendChild(document.createElement('tr'));

            const td1 = tr.appendChild(document.createElement('td'));
            td1.className = 'mlabels-left';
            td1.appendChild(innerTable);

            const td2 = tr.appendChild(document.createElement('td'));
            td2.className = 'mlabels-right';

            const span = td2.appendChild(document.createElement('span'));
            span.className = 'mlabels';
            span.appendChild(label);
        } else {
            const span = memproto.querySelector('table tr:first-child td:nth-child(2) span');
            if (span) {
                span.insertBefore(label, span.firstChild);
            }
        }
    }

    // For class/namespace/struct descriptions
    else if (div.parentElement.className === 'textblock') {
        note = document.createElement('dl');
        note.innerHTML = `<dl class="decompnote"><dt>Decompilation Note</dt><dd>${extendedText}</dd></dl>`;
        div.parentElement.insertBefore(note, div.parentElement.firstChild)
    }

    // For enum fields
    else if (div.parentElement.className === 'fielddoc') {

        // If the detail text is empty, fill it with a placeholder
        if (!div.previousElementSibling) {
            const p = document.createElement('p');
            p.textContent = shortText;
            div.parentElement.appendChild(p);
        } else {
            div.previousElementSibling.textContent += ' ' + shortText;
        }
    }

    // Remove the div
    div.remove();
}

document.querySelectorAll('div.unofficial').forEach(div => createCustomBubble(div, 'unofficial name', 'Unofficial name.', 'This element has an unofficial name.'));
document.querySelectorAll('div.unused').forEach(div => createCustomBubble(div, 'unused', 'Unused.', 'This element is unused.'));

//////////////////////////////////
// Github Style Markdown Alerts //
//////////////////////////////////

const alertIcons = {
    todo: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="M2.5 1.75v11.5c0 .138.112.25.25.25h3.17a.75.75 0 0 1 0 1.5H2.75A1.75 1.75 0 0 1 1 13.25V1.75C1 .784 1.784 0 2.75 0h8.5C12.216 0 13 .784 13 1.75v7.736a.75.75 0 0 1-1.5 0V1.75a.25.25 0 0 0-.25-.25h-8.5a.25.25 0 0 0-.25.25Zm13.274 9.537v-.001l-4.557 4.45a.75.75 0 0 1-1.055-.008l-1.943-1.95a.75.75 0 0 1 1.062-1.058l1.419 1.425 4.026-3.932a.75.75 0 1 1 1.048 1.074ZM4.75 4h4.5a.75.75 0 0 1 0 1.5h-4.5a.75.75 0 0 1 0-1.5ZM4 7.75A.75.75 0 0 1 4.75 7h2a.75.75 0 0 1 0 1.5h-2A.75.75 0 0 1 4 7.75Z"></path>
        </svg>`,
    warning: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="M6.457 1.047c.659-1.234 2.427-1.234 3.086 0l6.082 11.378A1.75 1.75 0 0 1 14.082 15H1.918a1.75 1.75 0 0 1-1.543-2.575Zm1.763.707a.25.25 0 0 0-.44 0L1.698 13.132a.25.25 0 0 0 .22.368h12.164a.25.25 0 0 0 .22-.368Zm.53 3.996v2.5a.75.75 0 0 1-1.5 0v-2.5a.75.75 0 0 1 1.5 0ZM9 11a1 1 0 1 1-2 0 1 1 0 0 1 2 0Z"></path>
        </svg>`,
    note: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="M0 8a8 8 0 1 1 16 0A8 8 0 0 1 0 8Zm8-6.5a6.5 6.5 0 1 0 0 13 6.5 6.5 0 0 0 0-13ZM6.5 7.75A.75.75 0 0 1 7.25 7h1a.75.75 0 0 1 .75.75v2.75h.25a.75.75 0 0 1 0 1.5h-2a.75.75 0 0 1 0-1.5h.25v-2h-.25a.75.75 0 0 1-.75-.75ZM8 6a1 1 0 1 1 0-2 1 1 0 0 1 0 2Z"></path>
        </svg>`,
    decompnote: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="m11.28 3.22 4.25 4.25a.75.75 0 0 1 0 1.06l-4.25 4.25a.749.749 0 0 1-1.275-.326.749.749 0 0 1 .215-.734L13.94 8l-3.72-3.72a.749.749 0 0 1 .326-1.275.749.749 0 0 1 .734.215Zm-6.56 0a.751.751 0 0 1 1.042.018.751.751 0 0 1 .018 1.042L2.06 8l3.72 3.72a.749.749 0 0 1-.326 1.275.749.749 0 0 1-.734-.215L.47 8.53a.75.75 0 0 1 0-1.06Z"></path>
        </svg>`,
    hint: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="M8 1.5c-2.363 0-4 1.69-4 3.75 0 .984.424 1.625.984 2.304l.214.253c.223.264.47.556.673.848.284.411.537.896.621 1.49a.75.75 0 0 1-1.484.211c-.04-.282-.163-.547-.37-.847a8.456 8.456 0 0 0-.542-.68c-.084-.1-.173-.205-.268-.32C3.201 7.75 2.5 6.766 2.5 5.25 2.5 2.31 4.863 0 8 0s5.5 2.31 5.5 5.25c0 1.516-.701 2.5-1.328 3.259-.095.115-.184.22-.268.319-.207.245-.383.453-.541.681-.208.3-.33.565-.37.847a.751.751 0 0 1-1.485-.212c.084-.593.337-1.078.621-1.489.203-.292.45-.584.673-.848.075-.088.147-.173.213-.253.561-.679.985-1.32.985-2.304 0-2.06-1.637-3.75-4-3.75ZM5.75 12h4.5a.75.75 0 0 1 0 1.5h-4.5a.75.75 0 0 1 0-1.5ZM6 15.25a.75.75 0 0 1 .75-.75h2.5a.75.75 0 0 1 0 1.5h-2.5a.75.75 0 0 1-.75-.75Z"></path>
        </svg>`,
    bug: `
        <svg class="markdown-alert-icon" viewBox="0 0 16 16" width="16" height="16" aria-hidden="true">
        <path d="M4.72.22a.75.75 0 0 1 1.06 0l1 .999a3.488 3.488 0 0 1 2.441 0l.999-1a.748.748 0 0 1 1.265.332.75.75 0 0 1-.205.729l-.775.776c.616.63.995 1.493.995 2.444v.327c0 .1-.009.197-.025.292.408.14.764.392 1.029.722l1.968-.787a.75.75 0 0 1 .556 1.392L13 7.258V9h2.25a.75.75 0 0 1 0 1.5H13v.5c0 .409-.049.806-.141 1.186l2.17.868a.75.75 0 0 1-.557 1.392l-2.184-.873A4.997 4.997 0 0 1 8 16a4.997 4.997 0 0 1-4.288-2.427l-2.183.873a.75.75 0 0 1-.558-1.392l2.17-.868A5.036 5.036 0 0 1 3 11v-.5H.75a.75.75 0 0 1 0-1.5H3V7.258L.971 6.446a.75.75 0 0 1 .558-1.392l1.967.787c.265-.33.62-.583 1.03-.722a1.677 1.677 0 0 1-.026-.292V4.5c0-.951.38-1.814.995-2.444L4.72 1.28a.75.75 0 0 1 0-1.06Zm.53 6.28a.75.75 0 0 0-.75.75V11a3.5 3.5 0 1 0 7 0V7.25a.75.75 0 0 0-.75-.75ZM6.173 5h3.654A.172.172 0 0 0 10 4.827V4.5a2 2 0 1 0-4 0v.327c0 .096.077.173.173.173Z"></path>
        </svg>`,
};

const validAlertIcons = Object.keys(alertIcons);

document.querySelectorAll('dl').forEach(dl => {

    // Get the class name
    const classes = dl.className.trim().split(/\s+/);
    const type = classes[classes.length - 1];
    if (!validAlertIcons.includes(type)) {
        return;
    }

    // Extract the text content
    const title = dl.querySelector('dt')?.innerHTML.trim();
    const content = dl.querySelector('dd')?.innerHTML.trim() || '';

    // Create the div and remove the dl
    const wrapper = document.createElement('div');
    wrapper.className = `markdown-alert markdown-alert-${type}`;
    wrapper.innerHTML = `<p class="markdown-alert-title">${alertIcons[type]}${title}</p><p>${content}</p>`;
    dl.replaceWith(wrapper);
});

////////////////////////////
// Remove "More..." Links //
////////////////////////////

document.querySelectorAll('a').forEach(link => {
    if (link.textContent.trim() === 'More...') {
        link.remove();
    }
});

///////////////////////////////////////
// Generate Template Parameter Table //
///////////////////////////////////////

document.querySelectorAll('dl.tparams').forEach(dl => {
    const dd = dl.querySelector('dd');
    const table = dd?.querySelector('table.tparams tbody');
    if (!table) {
        return;
    }

    // Create the heading
    const heading = document.createElement('h2');
    heading.className = 'groupheader';
    heading.innerHTML = `<a class="anchor" id="templ-params"></a>Template Parameters`;

    // Create the markdown table
    const newTable = document.createElement('table');
    newTable.className = 'markdownTable';
    const tbody = document.createElement('tbody');

    // Add header row
    const headerRow = document.createElement('tr');
    headerRow.className = 'markdownTableHead';
    headerRow.innerHTML = `
    <th class="markdownTableHeadCenter">Parameter</th>
    <th class="markdownTableHeadCenter">Description</th>
    `;
    tbody.appendChild(headerRow);

    // Add each parameter rows
    table.querySelectorAll('tr').forEach((row, idx) => {
        const cells = row.querySelectorAll('td');
        if (cells.length !== 2) {
            return;
        }

        const newRow = document.createElement('tr');
        newRow.className = 'markdownTableRowOdd';

        const paramCell = document.createElement('td');
        paramCell.className = 'markdownTableBodyCenter';

        // Check for dummy classes created by Doxygen
        const aTag = cells[0].querySelector('a');
        if (aTag) {
            paramCell.textContent = aTag.textContent.trim();
        } else {
            paramCell.innerHTML = cells[0].innerHTML.trim();
        }

        const descCell = document.createElement('td');
        descCell.className = 'markdownTableBodyCenter';
        descCell.innerHTML = cells[1].innerHTML.trim();

        newRow.appendChild(paramCell);
        newRow.appendChild(descCell);
        tbody.appendChild(newRow);
    });

    // Insert body in table
    newTable.appendChild(tbody);

    // Insert the heading and table
    dl.parentNode.insertBefore(document.createElement('br'), dl);
    dl.parentNode.insertBefore(heading, dl);
    dl.parentNode.insertBefore(newTable, dl);

    // Remove the old dl
    dl.remove();
});

///////////////////////////
// Create State ID table //
///////////////////////////

const stateIDList = [...document.querySelectorAll('.memberdecls .memItemRight a')].filter(row => {
    return row.innerText.startsWith('StateID_');
}).map(r => {
    const isInherited = r.closest('.inherit') != null;
    const name = r.innerText.replace('StateID_', '');
    const href = r.getAttribute('href');
    const id = href.split("#")[1];
    const descriptionElement = document.getElementsByClassName(`memdesc:${id}`);
    let description = '';
    if (descriptionElement.length == 1) {
        description = descriptionElement[0].querySelector('.mdescRight').innerText.trim();
    }
    return { name, description, href, isInherited };
});

const tablePlaceholder = document.getElementById('state-table-placeholder');
if (tablePlaceholder) {
    let stateIDTable = document.createElement('table');
    stateIDTable.className = 'doxtable';

    let stateIDTBody = document.createElement('tbody');
    stateIDTable.appendChild(stateIDTBody);

    let stateIDHeader = document.createElement('tr');
    stateIDHeader.innerHTML = `<th>State Name</th><th>Description</th>`;
    stateIDTBody.appendChild(stateIDHeader);

    stateIDList.forEach(state => {
        let row = document.createElement('tr');
        const inhText = state.isInherited ? ' (inherited)' : '';
        row.innerHTML = `<td><a href=${state.href}>${state.name}</a>${inhText}</td><td>${state.description}</td>`;
        stateIDTBody.appendChild(row);
    });

    tablePlaceholder.appendChild(stateIDTable);
}

////////////////////////////
// Create parameter table //
////////////////////////////

const paramDescInfo = new Map();
const paramEntries = new Array(32).fill(null).map(() => ({
    name: "",
    empty: true,
    isInherited: false
}));

// Find member rows
const memberRows = [...document.querySelectorAll('table.memberdecls tr[class^="memitem"]')];
memberRows.forEach(memItem => {
    const memRight = memItem.querySelector('.memItemRight');
    if (!memRight) {
        return;
    }
    const match = memRight.innerText.match(/ACTOR_PARAM_CONFIG\s*\(([^,]+),\s*([^,]+),\s*([^,]+)\)/);
    if (!match) {
        return;
    }

    const name = match[1];
    const offset = parseInt(match[2]);
    const size = parseInt(match[3]);
    const isInherited = memItem.closest('.inherit') != null;

    // Fill bit table
    for (let i = offset; i < offset + size; i++) {
        paramEntries[i] = {
            name: name,
            empty: false,
            isInherited
        };
    }

    // Grab description from following memdesc
    let descText = "";
    const next = memItem.nextElementSibling;
    if (next && next.className.startsWith('memdesc')) {
        const descTd = next.querySelector('.mdescRight');
        if (descTd) {
            descText = descTd.innerHTML;
        }
    }

    paramDescInfo.set(name, descText);

    // Remove memitem + memdesc
    memItem.remove();
    if (next && next.className.startsWith('memdesc')) {
        next.remove();
    }
});

const paramTablePlaceholder = document.getElementById('param-table-placeholder');

// Only generate if placeholder was found and at least one field is present
if (paramTablePlaceholder != null && paramEntries.some(e => !e.empty)) {

    /////////////////////
    // Bit usage table //
    /////////////////////

    let paramTable = document.createElement('table');
    paramTable.classList.add('doxtable', 'paramtable');

    let paramTBody = document.createElement('tbody');
    paramTable.appendChild(paramTBody);

    let paramHeader = document.createElement('tr');
    paramHeader.innerHTML = `<th colspan="16">Parameter usage</th>`;
    paramTBody.appendChild(paramHeader);

    for (let rowNum = 1; rowNum >= 0; rowNum--) {
        let paramHeaderRow = document.createElement('tr');
        for (let i = 15; i >= 0; i--) {
            let td = document.createElement('td');
            td.innerHTML = `${rowNum * 16 + i}`;
            paramHeaderRow.appendChild(td);
        }
        paramTBody.appendChild(paramHeaderRow);

        let paramRow = document.createElement('tr');
        let idx = 15;
        let currTD = document.createElement('td');

        while (idx >= 0) {
            const entry = paramEntries[rowNum * 16 + idx];

            // Handle fields spanning multiple columns
            let count = 0;
            do {
                idx--;
                count++;
            } while (idx >= 0 && paramEntries[rowNum * 16 + idx].name == entry.name);
            currTD.setAttribute('colspan', count);

            currTD.classList.add('param-name');
            currTD.innerHTML = entry.name;

            if (entry.isInherited) {
                currTD.classList.add('param-inherit');
            }

            if (entry.empty) {
                currTD.classList.add('param-empty');
                currTD.innerHTML = '&nbsp;';
            }

            paramRow.appendChild(currTD);
            currTD = document.createElement('td');
        }

        paramTBody.appendChild(paramRow);
    }

    paramTablePlaceholder.appendChild(paramTable);

    ///////////////////////
    // Description table //
    ///////////////////////

    let descTable = document.createElement('table');
    descTable.classList.add('doxtable', 'paramdesctable');

    let descBody = document.createElement('tbody');
    descTable.appendChild(descBody);

    let descHeader = document.createElement('tr');
    descHeader.innerHTML = `<th>Parameter</th><th>Description</th>`;
    descBody.appendChild(descHeader);

    for (const [name, desc] of paramDescInfo.entries()) {
        let row = document.createElement('tr');
        row.innerHTML = `
            <td class="param-name">${name}</td>
            <td>${desc || '<em>No description.</em>'}</td>
        `;
        descBody.appendChild(row);
    }

    paramTablePlaceholder.appendChild(descTable);

} else if (paramTablePlaceholder) {
    paramTablePlaceholder.innerHTML = '<em>No parameter fields available.</em>';
}

////////////////////////////////////////
// Fix Inherited Types Ignoring Close //
////////////////////////////////////////

document.querySelectorAll('tr.inherit_header').forEach((header, index, headers) => {
    const classList = Array.from(header.classList);
    const pubTypesClass = classList.find(c => c.startsWith('pub_types_'));
    if (!pubTypesClass) {
        return;
    }

    // Try to find the next inherit_header after this one
    for (let next = header.nextElementSibling; next; next = next.nextElementSibling) {
        if (next.classList.contains('inherit_header')) {
            break;
        }

        // Ensure classes are set
        next.classList.add('inherit', pubTypesClass);
    }
});

//////////////////////////////
// Generate TOC Dynamically //
//////////////////////////////

(function generateTOC() {

    // Remove all h1 headings
    document.querySelectorAll('h1').forEach(heading => heading.remove());

    // Find where to inject the TOC
    const contentDiv = document.querySelector('div.textblock');
    if (!contentDiv) {
        return;
    }

    // Find all relevant headings
    const headings = Array.from(document.querySelectorAll('h2, h3, h4, h5, h6')).filter(h => !h.classList.contains('memtitle') && h.firstElementChild);
    if (headings.length <= 1) {
        return;
    }

    // Create wrapper
    const tocContainer = document.createElement('div');
    tocContainer.className = 'toc interactive open';

    // TOC title
    const tocTitle = document.createElement('h3');
    tocTitle.textContent = 'Table of Contents';
    tocContainer.appendChild(tocTitle);

    // Add event listener for mobile closing/opening (from interactive TOC script)
    tocTitle.addEventListener('click', () => {
        if (tocTitle.parentElement.classList.contains('open')) {
            tocTitle.parentElement.classList.remove('open')
        } else {
            tocTitle.parentElement.classList.add('open')
        }
    });

    // Create basic TOC contents
    const topLevelUl = document.createElement('ul');
    const topLi = document.createElement('li');
    topLi.className = 'level1 empty';
    const nestedUl = document.createElement('ul');
    topLi.appendChild(nestedUl);
    topLevelUl.appendChild(topLi);
    tocContainer.appendChild(topLevelUl);

    const currentParents = { 2: nestedUl };

    headings.forEach(heading => {
        const level = parseInt(heading.tagName[1]);
        const anchor = heading.querySelector('a');
        if (!anchor) {
            return;
        }

        const href = anchor.getAttribute('id');
        const text = heading.textContent.trim();

        const link = document.createElement('a');
        link.href = `#${href}`;
        link.textContent = text;

        const li = document.createElement('li');
        li.className = `level${level}`;
        li.appendChild(link);

        // Ensure parent exists
        if (!currentParents[level - 1]) {
            currentParents[level - 1] = currentParents[level - 2] || nestedUl;
        }

        const parentUl = currentParents[level - 1];
        if (!parentUl) {
            return;
        }

        // Create and register child container
        parentUl.appendChild(li);
        const subUl = document.createElement('ul');
        li.appendChild(subUl);
        currentParents[level] = subUl;

        // Clear deeper levels
        for (let l = level + 1; l <= 6; l++) {
            delete currentParents[l];
        }
    });

    // Insert the TOC
    contentDiv.insertBefore(tocContainer, contentDiv.firstChild);

    // Add clearfix to ensure TOC does not oversize document
    const clearfix = document.createElement('div');
    clearfix.style.clear = 'both';
    contentDiv.appendChild(clearfix);
})();

///////////////////////////////////////////
// Generate Footer Navigator Dynamically //
///////////////////////////////////////////

(function getPagePath() {
    const navTree = document.getElementById('nav-tree');
    if (!navTree) {
        return;
    }

    let lastSelected = null;

    const updatePath = () => {
        const selected = document.querySelector('#nav-tree .selected');

        // Skip if nothing selected or selection hasn't changed
        if (!selected || selected === lastSelected) {
            return;
        }

        // Set up loop
        lastSelected = selected;
        const pathParts = [];
        let currentItem = selected;

        while (true) {

            // Add current text
            const spanLink = currentItem.querySelector('span > a');
            if (spanLink) {
                pathParts.unshift(spanLink);
            }

            // Find the parent item
            const ul = currentItem.parentElement.parentElement;
            let prev = ul.previousElementSibling;

            // Exit the loop if we reach the top of the tree
            if (!prev || !prev.classList.contains('item')) {
                break;
            }

            currentItem = prev;
        }

        // Remove the first element
        pathParts.shift();

        // Update the navpath bar
        const navPath = document.querySelector('.navpath ul');
        if (navPath) {

            // Remove existing elements
            navPath.querySelectorAll('li.navelem').forEach(li => li.remove());

            // Add new path elements
            pathParts.forEach(part => {
                const li = document.createElement('li');
                li.className = 'navelem';
                const clone = part.cloneNode(true);
                clone.className = 'el';
                li.appendChild(clone);
                navPath.appendChild(li);
            });
        }
    };

    // Wait for the navtree to change
    const observer = new MutationObserver(updatePath);
    observer.observe(navTree, {
        childList: true,
        attributeFilter: ['class'],
        subtree: true,
    });
})();
