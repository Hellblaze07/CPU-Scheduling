"use client";

import { PageContainer } from "@/components/layout/PageContainer";
import { Button } from "@/components/ui/button";
import { Card } from "@/components/ui/card";
import { ArrowRight, Cpu, GitBranch, MonitorPlay, Zap, RefreshCcw, Layers, TerminalSquare, Activity } from "lucide-react";
import Link from "next/link";
import { motion, Variants } from "framer-motion";
import { ProcessTypeBadge } from "@/components/process/ProcessTypeBadge";

// Motion variants
const staggerContainer: Variants = {
  hidden: { opacity: 0 },
  show: {
    opacity: 1,
    transition: {
      staggerChildren: 0.1,
    },
  },
};

const fadeUp: Variants = {
  hidden: { opacity: 0, y: 20 },
  show: { opacity: 1, y: 0, transition: { type: "spring", stiffness: 100, damping: 20 } },
};

export default function Home() {
  return (
    <PageContainer>
      <motion.div 
        initial="hidden"
        animate="show"
        variants={staggerContainer}
        className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 pt-16 pb-24"
      >
        {/* HERO SECTION - ASYMMETRIC SPLIT */}
        <div className="grid grid-cols-1 lg:grid-cols-12 gap-12 items-center min-h-[70vh]">
          {/* Left Text Block */}
          <div className="lg:col-span-6 space-y-8">
            <motion.div variants={fadeUp} className="inline-flex items-center rounded-full border border-zinc-800 bg-zinc-950/50 px-3 py-1 text-xs font-mono text-zinc-400">
              <span className="flex h-2 w-2 rounded-full bg-emerald-500 mr-2 animate-pulse"></span>
              v0.9.0 System Online
            </motion.div>
            
            <motion.div variants={fadeUp} className="space-y-4">
              <h1 className="text-5xl sm:text-7xl font-extrabold tracking-tighter text-zinc-100">
                Sched<span className="text-emerald-500">X</span>
              </h1>
              <p className="text-2xl font-medium text-zinc-400">
                Native CPU Scheduling.
                <br /> Visualized.
              </p>
            </motion.div>

            <motion.p variants={fadeUp} className="text-lg text-zinc-500 leading-relaxed max-w-lg">
              Watch how modern operating systems allocate CPU time. SchedX bridges classic CS theory and actual low-level C programming through an interactive, deterministic simulation engine.
            </motion.p>

            <motion.div variants={fadeUp} className="flex flex-col sm:flex-row gap-4 pt-4">
              <Link href="/simulator">
                <Button size="lg" className="h-14 px-8 text-lg gap-2 w-full sm:w-auto font-semibold bg-emerald-500 text-black hover:bg-emerald-400 transition-transform active:scale-95">
                  <MonitorPlay className="w-5 h-5" /> Launch Simulator
                </Button>
              </Link>
              <Link href="https://github.com/Hellblaze07/CPU-Scheduling" target="_blank" rel="noopener noreferrer">
                <Button variant="outline" size="lg" className="h-14 px-8 text-lg gap-2 w-full sm:w-auto font-semibold border-zinc-800 text-zinc-300 hover:bg-zinc-900 transition-transform active:scale-95">
                  <GitBranch className="w-5 h-5" /> View Source
                </Button>
              </Link>
            </motion.div>
          </div>

          {/* Right Visual Block - Mock Terminal */}
          <motion.div variants={fadeUp} className="lg:col-span-6 relative">
            <div className="absolute -inset-1 bg-gradient-to-r from-emerald-500/20 to-cyan-500/20 blur-2xl opacity-50 rounded-3xl" />
            <Card className="relative p-6 bg-[#0d1117] border-zinc-800 shadow-2xl rounded-2xl overflow-hidden group">
              <div className="flex items-center gap-2 mb-4">
                <div className="w-3 h-3 rounded-full bg-red-500/80" />
                <div className="w-3 h-3 rounded-full bg-yellow-500/80" />
                <div className="w-3 h-3 rounded-full bg-green-500/80" />
                <span className="ml-2 text-xs font-mono text-zinc-500">scheduler_engine.c</span>
              </div>
              <pre className="text-xs sm:text-sm text-[#c9d1d9] font-mono leading-loose overflow-x-auto">
                <span className="text-[#ff7b72]">struct</span> Process {'{\n'}
                {'  '}int pid;{'\n'}
                {'  '}int arrival_time;{'\n'}
                {'  '}int burst_time;{'\n'}
                {'  '}ProcessType type;{'\n'}
                {'}'};{'\n\n'}
                <span className="text-[#8b949e]">/* Adaptive Hybrid Evaluation */</span>{'\n'}
                <span className="text-[#ff7b72]">if</span> (proc.waiting_time &gt; AGING_THRESHOLD) {'{\n'}
                {'  '}promote_process(&amp;proc);{'\n'}
                {'  '}emit_event(<span className="text-[#a5d6ff]">"AGING"</span>, proc.pid);{'\n'}
                {'}'}
              </pre>
              
              {/* Overlay animated scanline */}
              <div className="absolute inset-0 bg-gradient-to-b from-transparent via-emerald-500/5 to-transparent opacity-0 group-hover:opacity-100 group-hover:animate-[scan_2s_ease-in-out_infinite]" />
            </Card>
          </motion.div>
        </div>

        {/* FLOW DIAGRAM - HORIZONTAL TO VERTICAL RESPONSIVE */}
        <motion.div variants={fadeUp} className="mt-32 mb-32 hidden md:flex items-center justify-center gap-6 text-zinc-500 font-medium text-sm">
          <div className="flex flex-col items-center gap-4 group">
            <div className="w-14 h-14 rounded-xl bg-zinc-900 border border-zinc-800 flex items-center justify-center shadow-lg group-hover:border-emerald-500/50 transition-colors">
              <TerminalSquare className="w-6 h-6 text-zinc-400 group-hover:text-emerald-400" />
            </div>
            <span>Native C Engine</span>
          </div>
          <div className="w-16 h-px bg-zinc-800" />
          <div className="flex flex-col items-center gap-4 group">
            <div className="w-14 h-14 rounded-xl bg-zinc-900 border border-zinc-800 flex items-center justify-center shadow-lg group-hover:border-[var(--color-process-rt)]/50 transition-colors">
              <Activity className="w-6 h-6 text-zinc-400 group-hover:text-[var(--color-process-rt)]" />
            </div>
            <span>Hybrid Logic</span>
          </div>
          <div className="w-16 h-px bg-zinc-800" />
          <div className="flex flex-col items-center gap-4 group">
            <div className="w-14 h-14 rounded-xl bg-zinc-900 border border-zinc-800 flex items-center justify-center shadow-lg group-hover:border-cyan-500/50 transition-colors">
              <Zap className="w-6 h-6 text-zinc-400 group-hover:text-cyan-400" />
            </div>
            <span>Node.js Bridge</span>
          </div>
          <div className="w-16 h-px bg-zinc-800" />
          <div className="flex flex-col items-center gap-4 group">
            <div className="w-14 h-14 rounded-xl bg-zinc-900 border border-zinc-800 flex items-center justify-center shadow-lg group-hover:border-[var(--color-process-int)]/50 transition-colors">
              <Layers className="w-6 h-6 text-zinc-400 group-hover:text-[var(--color-process-int)]" />
            </div>
            <span>React Interface</span>
          </div>
        </motion.div>

        {/* BENTO GRID FOR ALGORITHMS */}
        <div className="mt-24 mb-32">
          <motion.div variants={fadeUp} className="mb-12">
            <h2 className="text-3xl font-bold tracking-tight text-zinc-100">Deterministic Rules.</h2>
            <p className="text-zinc-500 mt-2 text-lg">The multi-level queue is governed by three strict policies.</p>
          </motion.div>

          <div className="grid grid-cols-1 md:grid-cols-3 gap-6 auto-rows-[250px]">
            {/* Bento 1: Large Span */}
            <motion.div variants={fadeUp} className="md:col-span-2 md:row-span-2">
              <Card className="h-full p-8 flex flex-col justify-between bg-zinc-900/50 border-zinc-800 hover:bg-zinc-900 transition-colors group relative overflow-hidden">
                <div className="absolute top-0 right-0 w-64 h-64 bg-[var(--color-process-batch)]/10 blur-3xl rounded-full -mr-20 -mt-20 group-hover:bg-[var(--color-process-batch)]/20 transition-colors" />
                <div className="space-y-4 relative z-10">
                  <ProcessTypeBadge type="BATCH" />
                  <h3 className="text-3xl font-bold text-zinc-100">Shortest Job First</h3>
                  <p className="text-zinc-400 text-lg max-w-md leading-relaxed">
                    Optimized for sheer throughput. Built-in aging prevents starvation by dynamically promoting long-waiting background jobs to Interactive status.
                  </p>
                </div>
                <div className="relative z-10 mt-8 flex items-center gap-4 text-sm font-mono text-zinc-500">
                  <div className="flex items-center gap-2"><Cpu className="w-4 h-4" /> 100% Util</div>
                  <div className="flex items-center gap-2"><RefreshCcw className="w-4 h-4" /> Dynamic Aging</div>
                </div>
              </Card>
            </motion.div>

            {/* Bento 2 */}
            <motion.div variants={fadeUp} className="md:col-span-1 md:row-span-1">
              <Card className="h-full p-6 flex flex-col justify-center space-y-3 bg-zinc-900/50 border-zinc-800 hover:bg-zinc-900 transition-colors relative overflow-hidden group">
                <div className="absolute top-0 right-0 w-32 h-32 bg-[var(--color-process-rt)]/10 blur-2xl rounded-full -mr-10 -mt-10 group-hover:bg-[var(--color-process-rt)]/20 transition-colors" />
                <ProcessTypeBadge type="REAL_TIME" />
                <h3 className="font-semibold text-xl text-zinc-100">Strict Priority</h3>
                <p className="text-sm text-zinc-400">
                  Preempts all other workloads. Always executes first when ready, minimizing response time.
                </p>
              </Card>
            </motion.div>

            {/* Bento 3 */}
            <motion.div variants={fadeUp} className="md:col-span-1 md:row-span-1">
              <Card className="h-full p-6 flex flex-col justify-center space-y-3 bg-zinc-900/50 border-zinc-800 hover:bg-zinc-900 transition-colors relative overflow-hidden group">
                <div className="absolute top-0 right-0 w-32 h-32 bg-[var(--color-process-int)]/10 blur-2xl rounded-full -mr-10 -mt-10 group-hover:bg-[var(--color-process-int)]/20 transition-colors" />
                <ProcessTypeBadge type="INTERACTIVE" />
                <h3 className="font-semibold text-xl text-zinc-100">Adaptive RR</h3>
                <p className="text-sm text-zinc-400">
                  Time quantum scales dynamically based on system load, balancing fairness and overhead.
                </p>
              </Card>
            </motion.div>
          </div>
        </div>

      </motion.div>
    </PageContainer>
  );
}
