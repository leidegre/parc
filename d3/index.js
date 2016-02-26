'use strict'

var particle = require('../src')
var d3 = require('d3')
var immutable = require('immutable')

function makeLayoutFriendlyRepresentation(state, nodes, links) {
  const node = nodes.get(state.id)
  if (node) {
    return [nodes, links]
  }
  nodes = nodes.set(state.id, { index: nodes.size, state })
  for (let transition of state.transitions_) {
    const next = makeLayoutFriendlyRepresentation(transition[1], nodes, links)
    nodes = next[0]
    links = next[1]
    links = links.push({
      source: nodes.get(state.id).index,
      target: nodes.get(transition[1].id).index,
      guard: transition[0]
    })
  }
  return [nodes, links]
}

var svg = null

function lineOffset(a, b, r) {
  const dx = b.x - a.x
  const dy = b.y - a.y
  const dr = Math.sqrt(dx * dx + dy * dy)
  const normalize = r / dr
  return { x: a.x + normalize * dx, y: a.y + normalize * dy }
}

function handleKeyup(e) {

  const dfa = particle.construct(e.target.value)
  const res = makeLayoutFriendlyRepresentation(dfa.dfa, immutable.Map(), immutable.List())

  const width = document.documentElement.clientWidth,
    height = document.documentElement.clientHeight,
    r = 30

  const force = d3.layout.force()
    .gravity(0.5)
    .charge(-300 * r)
    // .chargeDistance(10 * r)
    .linkDistance(15 * r)
    .size([width, height])

  // remember to clean up
  if (svg) {
    svg.remove()
  }
  svg = d3.select("body")
    .append("svg")
    .attr("style", 'position:fixed;top:0;left:0;z-index:-1001')
    .attr("width", width)
    .attr("height", height)

  svg.append("defs")
    .selectAll("marker")
    .data(["end"])
    .enter()
    .append("marker")
    .attr("id", String)
    .attr("viewBox", "-1 -1 2 2")
    .attr("refX", 0)
    .attr("refY", 0)
    .attr("markerWidth", r / 2)
    .attr("markerHeight", r / 2)
    .attr("orient", "auto")
    .append("path")
    .attr("d", "M -1,1 L 0,0 L -1,-1")
    .attr("stroke-width", "0.1")
    .attr("class", String)

  const nodes = res[0].toArray()
  const links = res[1].toArray()

  force
    .nodes(nodes)
    .links(links)
    .start()

  var link = svg.selectAll(".link")
    .data(links)
    .enter()

  var linkpath = link.append("path")
    .attr("marker-end", "url(#end)")
    .attr("class", "link")
    .style("fill", 'none')
    .style("stroke", '#999')

  var linktext = link.append("text")
    .attr("class", "label")
    .text((d) => d.guard.toString())

  var node = svg.selectAll(".node")
    .data(nodes)
    .enter()
    .append("g")
    .attr("class", "node")

  node.append("circle")
    .attr("r", r)
    .style("fill", '#fff')
    .style("stroke", '#000')

  node.append("text")
    .attr("class", "label")
    .text((d) => d.index)

  force.on("tick", function () {
    linkpath
      .attr("d", function (d) {
        if (d.source.index != d.target.index) {
          var u = lineOffset(d.source, d.target, r)
          var v = lineOffset(d.target, d.source, r)
          const dx = v.x - u.x
            , dy = v.y - u.y
            , dr = Math.sqrt(dx * dx + dy * dy)
          return `M ${u.x},${u.y} A ${dr},${dr} 0 0,1 ${v.x},${v.y}`
        } else {
          // self-reference, we could create a dummy node to act as a proxy for the self-reference 
          return null
        }
      })

    linktext
      .attr("transform", (d) => {
        const u = d.source
        const v = d.target
        const x = (u.x + v.x) / 2
        const y = (u.y + v.y) / 2
        return `translate(${x},${y})`
      })

    node
      .attr("transform", (d) => `translate(${d.x},${d.y})`)
  })
  
  // precompute 180 steps
  for (var i = 0; i < 180; i++) {
    force.tick()
  }
}

// init
var input = document.getElementById('input')
input.addEventListener('keyup', handleKeyup)
input.dispatchEvent(new Event('keyup'))

if (module.hot) {
  module.hot.accept()
  module.hot.dispose(() => {
    input.removeEventListener('keyup', handleKeyup)
    if (svg) {
      svg.remove()
    }
  })
}
