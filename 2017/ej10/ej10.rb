#!/usr/bin/env ruby
require 'git'
require 'logger'
require 'zlib'
require 'digest'
require 'pry'
require 'byebug'

MAX_LOGS = 1798

LOGGER = Logger.new(STDOUT)

# generator = PasswordGenerator.new(5985386, 9991722)
# generator.generate("hola", "dcb98e367896aff371efefe1b9f71b8b")
# puts generator.password, generator.digest
class PasswordGenerator
  attr_accessor :secret1, :secret2, :password, :digest, :counter

  def initialize(secret1, secret2)
    @secret1, @secret2 = [secret1, secret2]
  end

  def generate(userid, previous_hash = nil)
    reset_counter(userid, previous_hash)
    iterate_counter

    self.password = (1..10).map do
      (next_counter % 94 + 33).chr
    end.join

    self.digest = Digest::MD5.hexdigest(password)
  end

  private

  def iterate_counter
    self.counter = (counter * secret_generator) % secret2
  end

  def secret_generator
    return @secret_generator if @secret_generator

    exponent = 10000000
    result = 1
    modulus = secret2
    base = secret1 % modulus

    while exponent > 0
      if exponent % 2 == 1
         result = (result * base) % modulus
       end
      exponent = exponent >> 1
      base = (base * base) % modulus
    end
    @secret_generator = result
  end

  def reset_counter(userid, previous_hash = nil)
    self.counter = secret3(userid, previous_hash)
  end

  def secret3(userid, previous_hash = nil)
    secret3_base = previous_hash || userid
    Zlib::crc32(secret3_base)
  end

  def next_counter
    self.counter = (counter * secret1) % secret2
  end

end

class SecretsStore
  def initialize
    @hash = Hash.new
  end

  def store(date, secret1, secret2)
    key = date_key(date)
    @hash[date_key(date)] = {secret1: secret1, secret2: secret2}
  end

  # obtains using directly the key
  def obtain!(key)
    raise "Missing key #{key}" unless @hash.key?(key)
    @hash[key]
  end

  def contains?(date)
    @hash.key?(date_key(date))
  end

  def date_key(date)
    date.strftime("%Y-%m-%d")
  end

  def logger
    LOGGER
  end
end

class SecretExtractor
  def initialize
    @secret1_re = /\$secret1\s*=\s*(\d+);/
    @secret2_re = /\$secret2\s*=\s*(\d+);/
  end

  def extract(content)
    match1 = @secret1_re.match(content)
    match2 = @secret2_re.match(content)
    [match1[1].to_i, match2[1].to_i]
  end
end

class GitHistoryParser
  attr_accessor :secret_store, :extractor

  def initialize
    @secret_store = SecretsStore.new
    @extractor = SecretExtractor.new
  end

  def parse(repo)
    git = Git.open(repo)
    git.branches.local.each do |branch|
      logger.debug "Visiting branch #{branch.name}"
      git.checkout branch
      git.log(MAX_LOGS).each do |commit|
        next if secret_store.contains?(commit.date)
        content = git.show(commit.objectish, 'script.php')
        secret1, secret2 = extractor.extract(content)
        secret_store.store(commit.date, secret1, secret2)
      end
    end
    add_unreachables(git)
  end

  def add_unreachables(git)
    unreachables = git.lib.send(:command, 'fsck', ['--unreachable']).split("\n")
                      .select {|line| line.split[1] == "commit"}
                      .map {|line| line.split[2]}
    revlist = git.lib.send(:command, 'rev-list', ['--timestamp', '--no-walk', *unreachables])
                 .split("\n").map do |line|
                   line = line.split
                   {date: Time.at(line[0].to_i), id: line[1]}
                 end
    revlist.each do |rev|
      content = git.show(rev[:id]).split("\n").select {|x| x =~ /^\+/}.join("\n")
      secret1, secret2 = extractor.extract(content)
      secret_store.store(rev[:date], secret1, secret2)
    end
  end

  def logger
    LOGGER
  end
end

class Main
  def secret_store
    if File.exist?(secret_store_file)
      Marshal.load(File.read(secret_store_file))
    else
      parser = GitHistoryParser.new
      parser.parse('repo')
      secret_store = parser.secret_store
      File.write(secret_store_file, Marshal.dump(secret_store))
      secret_store
    end
  end

  def run(input)
    store = secret_store
    cases = input.readline.chomp.to_i

    (1..cases).each do |num_case|
      userid, entries = input.readline.chomp.split
      previous_hash, pg = nil
      (1..entries.to_i).each do
        date, iterations = input.readline.chomp.split
        secrets = store.obtain!(date)
        pg = PasswordGenerator.new(secrets[:secret1], secrets[:secret2])
        (1..iterations.to_i).each do
          pg.generate(userid, previous_hash)
          previous_hash = pg.digest
        end
      end
      puts "Case ##{num_case}: #{pg.password}"
    end
  end

  def secret_store_file
    'secret_store'
  end
end

main = Main.new
main.run(File.open('submitInput'))
