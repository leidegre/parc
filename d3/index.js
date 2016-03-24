'use strict'

var particle = require('../src')
var d3 = require('d3')
var immutable = require('immutable')
var Vector2D = require('./vector')

function makeLayoutFriendlyRepresentation(state, nodes, links) {
  const node = nodes.get(state.id)
  if (node) {
    return [nodes, links]
  }
  const source = nodes.size
  nodes = nodes.set(state.id, { index: source, state, accept: state.state_.find((v) => v.is_accept_state_) })
  for (let transition of state.transitions_) {
    const next = makeLayoutFriendlyRepresentation(transition[1], nodes, links)
    nodes = next[0]
    links = next[1]
    const target = nodes.get(transition[1].id).index
    if (source != target) {
      links = links.push({
        source: source,
        target: target,
        guard: transition[0]
      })
    } else {
      // transition through a dummy
      const dummy = nodes.size
      nodes = nodes.set("D" + state.id, { index: source })
      links = links.push({
        source: source,
        target: dummy,
        guard: transition[0]
      })
      links = links.push({
        source: dummy,
        target: target,
        guard: null
      })
    }
  }
  return [nodes, links]
}

var svg = null

function computeLinkCurve(u, v, r) {
  var a = Vector2D.from(u)
  var b = Vector2D.from(v)
  var d = b.sub(a)
  var n = d.normalize()
  var t = n.scale(r)
  var x = a.add(t) // offset by radius (from, to)
  var y = b.sub(t) // offset by radius (to, from)
  var m = Vector2D.lerp(x, y, 0.5) // midpoint
  var p = n.perp()
  var c = m.add(p.scale(r)) // curve control point
  var q = Vector2D.lerp(Vector2D.lerp(x, c, 0.5), Vector2D.lerp(c, y, 0.5), 0.5) // curve midpoint
  return {
    x, // from
    y, // to
    c, // curve control point
    q  // curve midpoint 
  }
}

function handleKeyup(e) {

  const dfa = particle.construct(e.target.value)
  const res = makeLayoutFriendlyRepresentation(dfa.dfa, immutable.Map(), immutable.List())

  const width = document.documentElement.clientWidth,
    height = document.documentElement.clientHeight,
    r = 30

  const force = d3.layout.force()
    .gravity(0.1)
    .charge((d) => -3000)
    .linkDistance((d) => 1 * r * (d.source.weight + d.target.weight))
    .linkStrength((d) => d.source.weight + d.target.weight)
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
    .style("stroke", '#ccc')

  var linktext = link.append("text")
    .attr("class", "label")
    .filter((d) => d.guard)
    .text((d) => d.guard.toString())

  var node = svg.selectAll(".node")
    .data(nodes)
    .enter()
    .append("g")
    .attr("class", "node")
    .call(force.drag)
    .on('click', (e) => {
      
      var id = e.state.state_.map((v) => v.is_accept_state_ ? `(${v.id_})` : `${v.id_}`).toArray().join()
      
      console.log(e, id)
    })

  node.filter((d) => d.state)
    .append("circle")
    .attr("r", r)
    .style("fill", '#fff')
    .style("stroke", '#000')

  node.filter((d) => d.accept)
    .append("circle")
    .attr("r", 0.9 * r)
    .style("fill", '#fff')
    .style("stroke", '#000')

  node.filter((d) => d.state)
    .append("text")
    .attr("class", "label")
    .text((d) => d.index)

  force.on("tick", function () {
    linkpath
      .attr("d", function (d) {
        const p = computeLinkCurve(d.source, d.target, r)
        const a = p.x
        const b = p.y
        const c = p.c
        return `M ${a.x},${a.y} Q ${c.x} ${c.y}, ${b.x},${b.y}`
      })

    linktext
      .attr("transform", (d) => {
        if (d.target.state) {
          const p = computeLinkCurve(d.source, d.target, r)
          const q = p.q
          return `translate(${q.x},${q.y})`
        }
        return `translate(${d.target.x},${d.target.y})`
      })

    node
      .attr("transform", (d) => `translate(${d.x},${d.y})`)
  })
  
  // precompute 180 steps
  for (var i = 0; i < 180; i++) {
    force.tick()
  }

  force.stop()
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
