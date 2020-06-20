import matplotlib.pyplot as plt
import json

RUNTIMES_FILE='ibm-nehalem.q_runtimes.json'

with open(RUNTIMES_FILE) as json_runtimes_file:
	implementations = json.load(json_runtimes_file)

for method in implementations['methods']:
	plt.plot(implementations['N'], method['runtimes'], label=method['name'])

plt.ylabel('Elapsed Time (s)')
plt.xlabel('N = Size of Matrix')
plt.title('Performance of the Implementations')
# Se limiteaza valoarea maxima pe Oy astfel incat sa fie mai vizibile ploturile
# pentru rularile optimizate
plt.ylim(top=implementations['y_top'])
plt.legend()

plt.show()
