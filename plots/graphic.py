import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np
import os
import pathlib
from matplotlib.ticker import LogLocator, ScalarFormatter

df = pd.read_excel("SPMV.xlsx")

for col in ['SIM', 'VLEN', 'LMUL', 'SIZE']:
    df[col] = df[col].astype('category')

df['VLEN'] = pd.Categorical(df['VLEN'],
                            categories=sorted(df['VLEN'].unique(), key=lambda x: int(x)),
                            ordered=True)

sns.set(style="whitegrid", font_scale=1.3)
output_dir = pathlib.Path(__file__).parent / "plots_spmv"
output_dir.mkdir(exist_ok=True)

palette = sns.color_palette("Set2", n_colors=len(df['LMUL'].cat.categories))

def plot_multi_panel(response):
    sims = df['SIM'].cat.categories
    sizes = df['SIZE'].cat.categories

    all_vlen_values = sorted([float(v) for v in df['VLEN'].cat.categories])

    fig, axes = plt.subplots(len(sims), len(sizes),
                             figsize=(8 * len(sizes), 4 * len(sims)),
                             sharey=True)

    if len(sims) == 1:
        axes = np.array([axes])
    if len(sizes) == 1:
        axes = axes.reshape(len(sims), 1)

    for i, sim in enumerate(sims):
        for j, size in enumerate(sizes):
            ax = axes[i, j]
            subset = df[(df['SIM'] == sim) & (df['SIZE'] == size)].copy()

            subset['VLEN'] = subset['VLEN'].astype(float)

            for k, lmul in enumerate(sorted(subset['LMUL'].unique(), key=lambda x: int(x))):
                data_lmul = subset[subset['LMUL'] == lmul]
                ax.plot(data_lmul['VLEN'], data_lmul[response],
                        marker='o', label=f'LMUL={lmul}', color=palette[k])

            ax.set_xscale('log', base=2)
            ax.xaxis.set_major_locator(LogLocator(base=2))
            ax.xaxis.set_major_formatter(ScalarFormatter())
            ax.xaxis.set_minor_locator(LogLocator(base=2, subs='auto'))
            ax.xaxis.set_minor_formatter(plt.NullFormatter())

            ax.set_xlim(min(all_vlen_values), max(all_vlen_values))
            ax.set_xticks(all_vlen_values)
            ax.set_xticklabels([str(int(v)) for v in all_vlen_values])

            ax.grid(True, which='major', linestyle='--', alpha=0.7)
            ax.grid(True, which='minor', linestyle=':', alpha=0.3)

            ax.set_xlabel("VLEN")
            ax.set_ylabel(response)
            ax.set_title(f"{sim}: SIZE = {size}")
            ax.legend(title="LMUL", loc='best', fontsize='small')

    plt.tight_layout()
    plt.savefig(output_dir / f"{response}.png", dpi=300)
    plt.close()





# === 4️⃣ Generazione grafici principali ===
for metric in ['cycle', 'instret']:
    plot_multi_panel(metric)

print(f"\n✅ Grafici salvati in '{output_dir}'")
