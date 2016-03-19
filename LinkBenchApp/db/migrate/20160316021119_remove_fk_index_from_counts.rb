class RemoveFkIndexFromCounts < ActiveRecord::Migration
  def change
	remove_index :counts, :link_id
  end
end
