#! /usr/bin/env python3

from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

DATA_DIR = Path('./data')
DATA_FILE = DATA_DIR / 'experimentacion.csv'

def read_data(path: Path = DATA_FILE) -> pd.DataFrame:
    data = pd.read_csv(DATA_FILE)
    data.threads = data.threads.apply(thread_to_cathegorical)
    return data


def thread_to_cathegorical(n: int) -> str:
    '''
    Renombra los campos de la columna thread a string. El campo `hue` de
    `seaborn.lineplot` falla cuando se le pasa una variable numerica, por eso
    se convierte el integer a una variable categorica.
    '''
    return f'{n} threads'


def pocos_threads(data: pd.DataFrame) -> pd.DataFrame:
    '''
    Devuelve un subset de `data` con solo los datos de 1, 2 o 4 threads.
    '''
    return data[data.threads.isin(['1 threads', '2 threads', '4 threads'])]


def plot(data: pd.DataFrame, grafo: str) -> None:
    print(f'graficando para {grafo}')
    data = data[data['grafo'] == grafo]
    ax = sns.lineplot(x='n', y='tiempo', hue='threads', data=data)
    return ax


if __name__ == '__main__':

    data = read_data()

    for grafo in data['grafo'].unique():
        plot(data, grafo)
        plt.savefig(DATA_DIR / f'{grafo}.png')
        plt.show()

    data = data[data.threads.isin(['1 threads', '2 threads', '4 threads'])]
    for grafo in data['grafo'].unique():
        plot(data, grafo)
        plt.savefig(DATA_DIR / f'{grafo}-124.png')
        plt.show()
