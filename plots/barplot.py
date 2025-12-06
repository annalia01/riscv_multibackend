import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_excel("Instret.xlsx")


print(df.head())

g = df.groupby("kernel").mean(numeric_only=True)


plt.figure(figsize=(12, 7))
plt.bar(g.index, g["instret"], color="purple")
plt.yscale("symlog", linthresh=100)
plt.xlabel("Kernel", fontsize=20)
plt.ylabel("instret", fontsize=20)
plt.title("Instret per Kernel", fontsize=24)

plt.xticks(rotation=45, fontsize=18)
plt.yticks(fontsize=18)

plt.tight_layout()
plt.savefig("instret_barplot.png", dpi=300)
plt.show()
