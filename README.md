# Classifiaction de documents

Programme classifiant des documents en plusieurs catégories. Le programme utilise un algorithme génétique, pour traiter un corpus de textes **français**.


### Installation

```sh
$ chmod +x install
$ install
```

### Exécution du programme

```sh
$ bin/classifier <nom du dossier> [--tdidf] [--luhn {0|1|2}] [--eucl] [--cos] [--help] [--usage]
```

Pour plus d'informations concernant l'utilisation du programme :
```sh
$ bin/classifier --help
```

# Exemple

```sh
$ chmod +x getData.sh
$ getData.sh # télécharger des données d'essai
$ bin/classifier data-test
```


## License

Le projet est sous licence gpl-3.0 - voir le fichier [LICENSE](LICENSE) pour plus de détails.
